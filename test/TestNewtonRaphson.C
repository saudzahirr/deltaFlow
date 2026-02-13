#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <Eigen/Dense>

#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

#include "Admittance.H"
#include "Logger.H"
#include "NewtonRaphson.H"
#include "Qlim.H"
#include "Reader.H"
#include "Writer.H"

TEST_CASE("Newton-Raphson 5-Bus Test", "[NewtonRaphson]") {
    DEBUG("Testing [NewtonRaphson] - 5 Bus System Power Flow ...");

    const int N = 5;

    // Bus data initialization
    BusData busData;
    busData.ID     = Eigen::VectorXi::LinSpaced(N, 1, N);
    busData.Type   = Eigen::VectorXi(N);
    busData.V      = Eigen::VectorXd::Constant(N, 1.0); // Initial guess
    busData.delta  = Eigen::VectorXd::Zero(N);
    busData.Pg     = Eigen::VectorXd::Zero(N);
    busData.Qg     = Eigen::VectorXd::Zero(N);
    busData.Pl     = Eigen::VectorXd::Zero(N);
    busData.Ql     = Eigen::VectorXd::Zero(N);
    busData.Qgmax  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Qgmin  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Gs     = Eigen::VectorXd::Zero(N);
    busData.Bs     = Eigen::VectorXd::Zero(N);

    // Define types: 1 = Slack, 2 = PV, 3 = PQ
    busData.Type << 1, 3, 2, 3, 3;

    // Set known data from your CSV
    busData.V(2) = 1.05; // PV bus voltage

    // PQ Load
    busData.Pl(1) = 8.0;
    busData.Ql(1) = 2.8;
    busData.Pl(2) = 0.8;
    busData.Ql(2) = 0.4;

    // Generator
    busData.Pg(2) = 5.2;
    busData.Qgmax(2) = 4.0;
    busData.Qgmin(2) = -2.8;

    // Branch data
    BranchData branchData;
    branchData.From     = Eigen::VectorXi(5);
    branchData.To       = Eigen::VectorXi(5);
    branchData.R        = Eigen::VectorXd(5);
    branchData.X        = Eigen::VectorXd(5);
    branchData.B        = Eigen::VectorXd(5);
    branchData.tapRatio = Eigen::VectorXd::Constant(5, 1.0);

    branchData.From << 1, 2, 2, 3, 4;
    branchData.To   << 5, 4, 5, 4, 5;
    branchData.R    << 0.0015, 0.009, 0.0045, 0.00075, 0.00225;
    branchData.X    << 0.02,   0.1,   0.05,   0.01,    0.025;
    branchData.B    << 0.00,   1.72,  0.88,   0.00,    0.44;

    Eigen::MatrixXcd Y = computeAdmittanceMatrix(busData, branchData);
    Eigen::MatrixXd G = Y.array().real().matrix();
    Eigen::MatrixXd B = Y.array().imag().matrix();

    // Flat start: PQ V=1.0, PV keep specified V, all delta=0
    Eigen::VectorXd V(N);
    Eigen::VectorXd delta = Eigen::VectorXd::Zero(N);
    for (int i = 0; i < N; ++i) {
        V(i) = (busData.Type(i) == 3) ? 1.0 : busData.V(i);
    }

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
            N, static_cast<int>(pq_indices.size()), pq_indices, 1024, 1E-8);

        if (!converged) break;

        Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
            busData, pv_indices, N);
    }

    // Post-convergence: update busData
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

    REQUIRE(converged);

    // Voltages
    REQUIRE(busData.V(0) == Catch::Approx(1.0000000000000000).margin(1E-12));
    REQUIRE(busData.V(1) == Catch::Approx(0.8337678171370211).margin(1E-12));
    REQUIRE(busData.V(2) == Catch::Approx(1.0500000000000000).margin(1E-12));
    REQUIRE(busData.V(3) == Catch::Approx(1.0193022826993177).margin(1E-12));
    REQUIRE(busData.V(4) == Catch::Approx(0.9742884694433818).margin(1E-12));

    // Angles in degrees
    REQUIRE(busData.delta(0) == Catch::Approx(0.0).margin(1E-12));
    REQUIRE(busData.delta(1) == Catch::Approx(-22.40641804643159).margin(1E-9));
    REQUIRE(busData.delta(2) == Catch::Approx(-0.5973464891581161).margin(1E-9));
    REQUIRE(busData.delta(3) == Catch::Approx(-2.833974239764798).margin(1E-9));
    REQUIRE(busData.delta(4) == Catch::Approx(-4.547884420849281).margin(1E-9));

    // Generator outputs
    REQUIRE(busData.Pg(0) == Catch::Approx(3.948387578413601).margin(1E-12));
    REQUIRE(busData.Qg(0) == Catch::Approx(1.142829005432617).margin(1E-12));
    REQUIRE(busData.Pg(2) == Catch::Approx(5.200000000000000).margin(1E-12));
    REQUIRE(busData.Qg(2) == Catch::Approx(3.374796297950904).margin(1E-12));

    // Loss checks
    double PtotalGen = busData.Pg.sum();
    double PtotalLoad = busData.Pl.sum();
    double QtotalGen = busData.Qg.sum();
    double QtotalLoad = busData.Ql.sum();

    REQUIRE((PtotalGen - PtotalLoad) > 0.0);
    REQUIRE((QtotalGen - QtotalLoad) > 0.0);
}
