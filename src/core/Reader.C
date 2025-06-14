#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Logger.H"
#include "Reader.H"

bool readBusDataCSV(const std::string& busCsvFilepath, BusData& busData) {
    std::ifstream file(busCsvFilepath);
    if (!file.is_open()) {
        ERROR("Unable to open bus data CSV file: {}", busCsvFilepath);
        return false;
    }

    std::string line;
    std::getline(file, line);

    std::vector<int> ids, types;
    std::vector<std::string> names;
    std::vector<double> V, delta, Pg, Qg, Pl, Ql, Qgmax, Qgmin, Gs, Bs;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, ',');
        ids.push_back(std::stoi(field));

        std::getline(ss, field, ',');
        names.push_back(field);

        std::getline(ss, field, ',');
        types.push_back(std::stoi(field));

        auto readDouble = [&](std::vector<double>& vec) {
            std::getline(ss, field, ',');
            vec.push_back(field.empty() ? 0.0 : std::stod(field));
        };

        readDouble(V); readDouble(delta); readDouble(Pg); readDouble(Qg);
        readDouble(Pl); readDouble(Ql); readDouble(Qgmax); readDouble(Qgmin);
        readDouble(Gs); readDouble(Bs);
    }

    size_t n = ids.size();
    busData.ID     = Eigen::Map<Eigen::VectorXi>(ids.data(), n);
    busData.Name   = names;
    busData.Type   = Eigen::Map<Eigen::VectorXi>(types.data(), n);
    busData.V      = Eigen::Map<Eigen::VectorXd>(V.data(), n);
    busData.delta  = Eigen::Map<Eigen::VectorXd>(delta.data(), n);
    busData.Pg     = Eigen::Map<Eigen::VectorXd>(Pg.data(), n);
    busData.Qg     = Eigen::Map<Eigen::VectorXd>(Qg.data(), n);
    busData.Pl     = Eigen::Map<Eigen::VectorXd>(Pl.data(), n);
    busData.Ql     = Eigen::Map<Eigen::VectorXd>(Ql.data(), n);
    busData.Qgmax  = Eigen::Map<Eigen::VectorXd>(Qgmax.data(), n);
    busData.Qgmin  = Eigen::Map<Eigen::VectorXd>(Qgmin.data(), n);
    busData.Gs     = Eigen::Map<Eigen::VectorXd>(Gs.data(), n);
    busData.Bs     = Eigen::Map<Eigen::VectorXd>(Bs.data(), n);

    return true;
}

bool readBranchDataCSV(const std::string& branchCsvFilepath, BranchData& branchData) {
    std::ifstream file(branchCsvFilepath);
    if (!file.is_open()) {
        std::cerr << "Unable to open branch data CSV file: " << branchCsvFilepath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);

    std::vector<int> fromVec, toVec;
    std::vector<double> R, X, G, B, a;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, ',');
        fromVec.push_back(std::stoi(field));

        std::getline(ss, field, ',');
        toVec.push_back(std::stoi(field));

        auto readDouble = [&](std::vector<double>& vec) {
            std::getline(ss, field, ',');
            vec.push_back(field.empty() ? 0.0 : std::stod(field));
        };

        readDouble(R);
        readDouble(X);
        readDouble(G);
        readDouble(B);
        readDouble(a);
    }

    size_t n = fromVec.size();

    branchData.From = Eigen::VectorXi::Map(fromVec.data(), n);
    branchData.To = Eigen::VectorXi::Map(toVec.data(), n);
    branchData.R = Eigen::VectorXd::Map(R.data(), n);
    branchData.X = Eigen::VectorXd::Map(X.data(), n);
    branchData.G = Eigen::VectorXd::Map(G.data(), n);
    branchData.B = Eigen::VectorXd::Map(B.data(), n);
    branchData.tapRatio = Eigen::VectorXd::Map(a.data(), n);

    return true;
}

void dispBusData(const BusData& busData) {
    int nbus = busData.V.size();

    INFO("Bus Data Summary");
    INFO("Bus  Voltage  Angle    ------Load------    ---Generation---   Injected");
    INFO("No.  Mag.     Degree     MW       Mvar       MW       Mvar       Mvar");
    INFO("{}", std::string(80, '='));

    for (int i = 0; i < nbus; ++i) {
        double injectedMvar = busData.Qg(i) - busData.Ql(i);

        INFO("{:>4d} {:9.4f} {:9.4f} {:10.4f} {:10.4f} {:10.4f} {:10.4f} {:10.4f}",
             i + 1,
             busData.V(i),
             busData.delta(i),
             busData.Pl(i),
             busData.Ql(i),
             busData.Pg(i),
             busData.Qg(i),
             injectedMvar);
    }

    double totalPl = busData.Pl.sum();
    double totalQl = busData.Ql.sum();
    double totalPg = busData.Pg.sum();
    double totalQg = busData.Qg.sum();
    double totalInjected = totalQg - totalQl;

    INFO("{}", std::string(80, '='));
    INFO("Total{:>30.4f}{:10.4f}{:10.4f}{:10.4f}{:10.4f}",
         totalPl, totalQl, totalPg, totalQg, totalInjected);
    INFO("");
}

void dispLineFlow(
    const BusData& busData,
    const BranchData& branchData,
    const Eigen::MatrixXcd& Y,
    double basemva
) {
    auto Bc = branchData.B;
    int nBus = static_cast<int>(busData.V.size());
    int nLine = static_cast<int>(branchData.From.size());

    Eigen::VectorXcd V(nBus);
    Eigen::VectorXcd S(nBus);

    for (int i = 0; i < V.size(); ++i) {
        double mag = busData.V(i);
        double ang_rad = busData.delta(i) * M_PI / 180.0;
        V(i) = std::polar(mag, ang_rad);

        double P = busData.Pg(i) - busData.Pl(i);
        double Q = busData.Qg(i) - busData.Ql(i);
        S(i) = std::complex<double>(P, Q);
    }



    std::complex<double> SLT = 0.0;

    INFO("");
    INFO("Line Flow and Losses");
    INFO("{}", std::string(80, '='));
    INFO("--Line--  Power at bus & line flow    --Line loss--  Transformer");
    INFO("from  to    MW      Mvar     MVA       MW      Mvar      tap");
    INFO("{}", std::string(80, '='));

    for (int n = 1; n <= nBus; ++n) {
        int n_idx = n - 1;
        bool busprt = false;

        for (int L = 0; L < nLine; ++L) {
            if (!busprt) {
                double P_inj = busData.Pg(n_idx) - busData.Pl(n_idx);
                double Q_inj = busData.Qg(n_idx) - busData.Ql(n_idx);
                double S_mag = std::abs(S(n_idx)) * basemva;

                INFO("{:6d}      {:9.3f} {:9.3f} {:9.3f}", n, P_inj, Q_inj, S_mag);
                busprt = true;
            }

            if (branchData.From(L) == n) {
                int k = branchData.To(L);
                int k_idx = k - 1;
                double aL = (branchData.tapRatio(L) == 0.0) ? 1.0 : branchData.tapRatio(L);

                std::complex<double> In = (V(n_idx) - aL * V(k_idx)) * Y(L) / (aL * aL) + Bc(L) / (aL * aL) * V(n_idx);
                std::complex<double> Ik = (V(k_idx) - V(n_idx) / aL) * Y(L) + Bc(L) * V(k_idx);

                std::complex<double> Snk = V(n_idx) * std::conj(In) * basemva;
                std::complex<double> Skn = V(k_idx) * std::conj(Ik) * basemva;
                std::complex<double> SL = Snk + Skn;

                SLT += SL;

                if (aL != 1.0) {
                    INFO("{:12d} {:9.3f} {:9.3f} {:9.3f} {:9.3f} {:9.3f} {:9.3f}",
                         k,
                         std::real(Snk),
                         std::imag(Snk),
                         std::abs(Snk),
                         std::real(SL),
                         std::imag(SL),
                         aL);
                } else {
                    INFO("{:12d} {:9.3f} {:9.3f} {:9.3f} {:9.3f} {:9.3f}",
                         k,
                         std::real(Snk),
                         std::imag(Snk),
                         std::abs(Snk),
                         std::real(SL),
                         std::imag(SL));
                }

            } else if (branchData.To(L) == n) {
                int k = branchData.From(L);
                int k_idx = k - 1;
                double aL = (branchData.tapRatio(L) == 0.0) ? 1.0 : branchData.tapRatio(L);

                std::complex<double> In = (V(n_idx) - V(k_idx) / aL) * Y(L) + Bc(L) * V(n_idx);
                std::complex<double> Ik = (V(k_idx) - aL * V(n_idx)) * Y(L) / (aL * aL) + Bc(L) / (aL * aL) * V(k_idx);

                std::complex<double> Snk = V(n_idx) * std::conj(In) * basemva;
                std::complex<double> Skn = V(k_idx) * std::conj(Ik) * basemva;
                std::complex<double> SL = Snk + Skn;

                SLT += SL;

                INFO("{:12d} {:9.3f} {:9.3f} {:9.3f} {:9.3f} {:9.3f}",
                     k,
                     std::real(Snk),
                     std::imag(Snk),
                     std::abs(Snk),
                     std::real(SL),
                     std::imag(SL));
            }
        }
    }

    SLT /= 2.0;

    INFO("");
    INFO("Total loss                         {:9.3f} {:9.3f}", std::real(SLT), std::imag(SLT));
}
