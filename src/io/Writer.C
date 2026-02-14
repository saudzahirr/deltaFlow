/*
 * Copyright (c) 2024 Saud Zahir
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with deltaFlow.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <complex>
#include <fstream>
#include <iomanip>

#include "Banner.H"
#include "Data.H"
#include "Logger.H"
#include "Writer.H"


void dispBusData(const BusData& busData) {
    int nbus = busData.V.size();

    Banner::printSectionHeader("B U S   D A T A   R E S U L T S");

    fmt::print(fg(Banner::BRAND_COLOR), "   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "Bus", "Voltage", "Angle", "Load", "Load", "Gen", "Gen", "Injected");
    fmt::print(fg(Banner::BRAND_COLOR), "   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "No.", "Mag.", "Degree", "MW", "Mvar", "MW", "Mvar", "Mvar");
    fmt::print("   {}\n", std::string(76, '='));

    for (int i = 0; i < nbus; ++i) {
        double injectedMvar = busData.Qg(i) - busData.Ql(i);

        fmt::print("   {:>4d}  {:>9.4f}  {:>9.4f}  {:>10.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
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

    fmt::print("   {}\n", std::string(76, '='));
    fmt::print("   Total{:>27.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
         totalPl, totalQl, totalPg, totalQg, totalInjected);
    fmt::print("\n");

    // Also log to file
    LOG_INFO("Bus Data Summary: {} buses", nbus);
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

    Banner::printSectionHeader("L I N E   F L O W   A N D   L O S S E S");

    fmt::print(fg(Banner::BRAND_COLOR), "   {:>4s}  {:>4s}  {:>9s} {:>9s} {:>9s}   {:>9s} {:>9s}  {:>9s}\n",
        "From", "To", "MW", "Mvar", "MVA", "Loss MW", "Loss Mvar", "Tap");
    fmt::print("   {}\n", std::string(76, '='));

    for (int n = 1; n <= nBus; ++n) {
        int n_idx = n - 1;
        bool busprt = false;

        for (int L = 0; L < nLine; ++L) {
            if (!busprt) {
                double P_inj = busData.Pg(n_idx) - busData.Pl(n_idx);
                double Q_inj = busData.Qg(n_idx) - busData.Ql(n_idx);
                double S_mag = std::abs(S(n_idx)) * basemva;

                fmt::print("   {:>4d}        {:>9.3f} {:>9.3f} {:>9.3f}\n", n, P_inj, Q_inj, S_mag);
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
                    fmt::print("         {:>4d}  {:>9.3f} {:>9.3f} {:>9.3f}   {:>9.3f} {:>9.3f}  {:>9.3f}\n",
                         k,
                         std::real(Snk),
                         std::imag(Snk),
                         std::abs(Snk),
                         std::real(SL),
                         std::imag(SL),
                         aL);
                } else {
                    fmt::print("         {:>4d}  {:>9.3f} {:>9.3f} {:>9.3f}   {:>9.3f} {:>9.3f}\n",
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

                fmt::print("         {:>4d}  {:>9.3f} {:>9.3f} {:>9.3f}   {:>9.3f} {:>9.3f}\n",
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

    fmt::print("\n");
    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold,
        "   Total loss                        {:>9.3f} {:>9.3f}\n",
        std::real(SLT), std::imag(SLT));
    fmt::print("\n");

    // Log summary
    LOG_INFO("Line Flow computed: Total loss P={:.3f} MW, Q={:.3f} Mvar",
        std::real(SLT), std::imag(SLT));
}

bool writeOutputCSV(const BusData& busData) {
    std::ofstream out("deltaFlow.csv");
    if (!out.is_open()) return false;

    out << "BusID,Name,Type,Voltage,Angle,Pg,Qg,Pl,Ql,Qgmax,Qgmin,Gs,Bs\n";
    for (int i = 0; i < busData.ID.size(); ++i) {
        out << busData.ID[i] << ","
            << busData.Name[i] << ","
            << busData.Type[i] << ","
            << std::fixed << std::setprecision(64)
            << busData.V[i] << ","
            << busData.delta[i] << ","
            << busData.Pg[i] << ","
            << busData.Qg[i] << ","
            << busData.Pl[i] << ","
            << busData.Ql[i] << ","
            << busData.Qgmax[i] << ","
            << busData.Qgmin[i] << ","
            << busData.Gs[i] << ","
            << busData.Bs[i] << "\n";
    }

    out.close();

    return true;
}
