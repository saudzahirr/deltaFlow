#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <Eigen/Dense>
#include <cmath>
#include <complex>
#include <string>
#include <vector>

#include "Admittance.H"
#include "GaussSeidel.H"
#include "NewtonRaphson.H"
#include "Qlim.H"
#include "Reader.H"
#include "Writer.H"

// ---------------------------------------------------------------------------
//  Test data directory
// ---------------------------------------------------------------------------

inline std::string testDataDir(const std::string& subdir = "IEEE") {
    std::string file(__FILE__);
    auto pos = file.rfind('/');
    return (pos != std::string::npos ? file.substr(0, pos) : ".") + "/data/" + subdir + "/";
}

// ---------------------------------------------------------------------------
//  5-Bus test system (used by Admittance, Newton-Raphson, Gauss-Seidel tests)
// ---------------------------------------------------------------------------

inline BusData create5BusBusData() {
    const int N = 5;

    BusData busData;
    busData.ID     = Eigen::VectorXi::LinSpaced(N, 1, N);
    busData.Type   = Eigen::VectorXi(N);
    busData.V      = Eigen::VectorXd::Constant(N, 1.0);
    busData.delta  = Eigen::VectorXd::Zero(N);
    busData.Pg     = Eigen::VectorXd::Zero(N);
    busData.Qg     = Eigen::VectorXd::Zero(N);
    busData.Pl     = Eigen::VectorXd::Zero(N);
    busData.Ql     = Eigen::VectorXd::Zero(N);
    busData.Qgmax  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Qgmin  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Gs     = Eigen::VectorXd::Zero(N);
    busData.Bs     = Eigen::VectorXd::Zero(N);

    // Bus types: 1 = Slack, 2 = PV, 3 = PQ
    busData.Type << 1, 3, 2, 3, 3;
    busData.V(2) = 1.05; // PV bus voltage

    // Loads
    busData.Pl(1) = 8.0;
    busData.Ql(1) = 2.8;
    busData.Pl(2) = 0.8;
    busData.Ql(2) = 0.4;

    // Generation
    busData.Pg(2) = 5.2;
    busData.Qgmax(2) = 4.0;
    busData.Qgmin(2) = -2.8;

    return busData;
}

inline BranchData create5BusBranchData() {
    const int nBranch = 5;

    BranchData branchData;
    branchData.From     = Eigen::VectorXi(nBranch);
    branchData.To       = Eigen::VectorXi(nBranch);
    branchData.R        = Eigen::VectorXd(nBranch);
    branchData.X        = Eigen::VectorXd(nBranch);
    branchData.B        = Eigen::VectorXd(nBranch);
    branchData.tapRatio = Eigen::VectorXd::Constant(nBranch, 1.0);

    branchData.From << 1, 2, 2, 3, 4;
    branchData.To   << 5, 4, 5, 4, 5;
    branchData.R    << 0.0015, 0.009, 0.0045, 0.00075, 0.00225;
    branchData.X    << 0.02,   0.1,   0.05,   0.01,    0.025;
    branchData.B    << 0.00,   1.72,  0.88,   0.00,    0.44;

    return branchData;
}

// ---------------------------------------------------------------------------
//  Post-convergence power calculation and busData update
// ---------------------------------------------------------------------------

inline void postProcess(
    BusData& busData,
    const BranchData& branchData,
    const Eigen::MatrixXcd& Y,
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta
) {
    int N = busData.ID.size();

    Eigen::VectorXcd Vc(N);
    for (int i = 0; i < N; ++i)
        Vc(i) = std::polar(V(i), delta(i));

    Eigen::VectorXd P_net = busData.Pg - busData.Pl;
    Eigen::VectorXd Q_net = busData.Qg - busData.Ql;

    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 1) {
            std::complex<double> Ii = Y.row(i) * Vc;
            std::complex<double> Si = Vc(i) * std::conj(Ii);
            P_net(i) = Si.real();
            Q_net(i) = Si.imag();
        }
    }
    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 2) {
            std::complex<double> Ii = Y.row(i) * Vc;
            Q_net(i) = -std::imag(std::conj(Vc(i)) * Ii);
        }
    }
    for (int i = 0; i < N; ++i) {
        busData.V(i) = std::abs(Vc(i));
        busData.delta(i) = std::arg(Vc(i)) * 180.0 / M_PI;
        busData.Pg(i) = P_net(i) + busData.Pl(i);
        busData.Qg(i) = Q_net(i) + busData.Ql(i);
    }

    dispBusData(busData);
    dispLineFlow(busData, branchData, Y);
}

// ---------------------------------------------------------------------------
//  Newton-Raphson power flow with Q-limit enforcement
// ---------------------------------------------------------------------------

inline bool solvePowerFlowNR(
    BusData& busData,
    const BranchData& branchData,
    int maxIter = 1024,
    double tol = 1E-8
) {
    int N = busData.ID.size();

    auto Y = computeAdmittanceMatrix(busData, branchData);
    Eigen::MatrixXd G = Y.array().real().matrix();
    Eigen::MatrixXd B = Y.array().imag().matrix();

    // Flat start
    Eigen::VectorXd V(N);
    Eigen::VectorXd delta = Eigen::VectorXd::Zero(N);
    for (int i = 0; i < N; ++i)
        V(i) = (busData.Type(i) == 3) ? 1.0 : busData.V(i);

    Eigen::VectorXi type_bus = busData.Type;

    // Outer Q-limit loop
    bool Q_lim_status = true;
    bool converged = false;

    while (Q_lim_status) {
        Eigen::VectorXd Ps = busData.Pg - busData.Pl;
        Eigen::VectorXd Qs = busData.Qg - busData.Ql;

        std::vector<int> pq_indices, pv_indices;
        for (int i = 0; i < N; ++i) {
            if (type_bus(i) == 3) pq_indices.push_back(i);
            else if (type_bus(i) == 2) pv_indices.push_back(i);
        }

        converged = NewtonRaphson(G, B, Ps, Qs, V, delta,
            N, static_cast<int>(pq_indices.size()), pq_indices, maxIter, tol);

        if (!converged) break;

        Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
            busData, pv_indices, N);
    }

    postProcess(busData, branchData, Y, V, delta);
    return converged;
}

// ---------------------------------------------------------------------------
//  Gauss-Seidel power flow with Q-limit enforcement
// ---------------------------------------------------------------------------

inline bool solvePowerFlowGS(
    BusData& busData,
    const BranchData& branchData,
    int maxIter = 1024,
    double tol = 1E-8,
    double alpha = 1.0
) {
    int N = busData.ID.size();

    auto Y = computeAdmittanceMatrix(busData, branchData);
    Eigen::MatrixXd G = Y.array().real().matrix();
    Eigen::MatrixXd B = Y.array().imag().matrix();

    // Flat start
    Eigen::VectorXd V(N);
    Eigen::VectorXd delta = Eigen::VectorXd::Zero(N);
    for (int i = 0; i < N; ++i)
        V(i) = (busData.Type(i) == 3) ? 1.0 : busData.V(i);

    Eigen::VectorXi type_bus = busData.Type;

    // Outer Q-limit loop
    bool Q_lim_status = true;
    bool converged = false;

    while (Q_lim_status) {
        Eigen::VectorXd Ps = busData.Pg - busData.Pl;
        Eigen::VectorXd Qs = busData.Qg - busData.Ql;

        std::vector<int> pv_indices;
        for (int i = 0; i < N; ++i)
            if (type_bus(i) == 2) pv_indices.push_back(i);

        converged = GaussSeidel(Y, V, delta, type_bus, Ps, Qs, N,
                                 maxIter, tol, alpha);

        if (!converged) break;

        Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
                                     busData, pv_indices, N);
    }

    postProcess(busData, branchData, Y, V, delta);
    return converged;
}

#endif // TEST_UTILS_H
