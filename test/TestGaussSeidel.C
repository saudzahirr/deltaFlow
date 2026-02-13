#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <Eigen/Dense>

#include <cmath>
#include <complex>
#include <iostream>

#include "Admittance.H"
#include "GaussSeidel.H"
#include "Logger.H"
#include "Reader.H"
#include "Writer.H"

TEST_CASE("Gauss-Seidel 5-Bus Power Flow Test", "[GaussSeidel]") {
    DEBUG("Testing [GaussSeidel] - 5 Bus System Power Flow ...");

    const int N = 5;

    // Setup bus data
    BusData busData;
    busData.ID     = Eigen::VectorXi::LinSpaced(N, 1, N);
    busData.Type   = Eigen::VectorXi(N);
    busData.V      = Eigen::VectorXd::Constant(N, 1.0);  // Initial guess
    busData.delta  = Eigen::VectorXd::Zero(N);           // Degrees
    busData.Pg     = Eigen::VectorXd::Zero(N);
    busData.Qg     = Eigen::VectorXd::Zero(N);
    busData.Pl     = Eigen::VectorXd::Zero(N);
    busData.Ql     = Eigen::VectorXd::Zero(N);
    busData.Qgmax  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Qgmin  = Eigen::VectorXd::Constant(N, 0.0);
    busData.Gs     = Eigen::VectorXd::Zero(N);
    busData.Bs     = Eigen::VectorXd::Zero(N);

    // Bus types
    busData.Type << 1, 3, 2, 3, 3;

    // Set PV bus voltage
    busData.V(2) = 1.05;

    // Set loads and generation
    busData.Pl(1) = 8.0;
    busData.Ql(1) = 2.8;

    busData.Pl(2) = 0.8;
    busData.Ql(2) = 0.4;
    busData.Pg(2) = 5.2;
    busData.Qgmax(2) = 4.0;
    busData.Qgmin(2) = -2.8;

    // Setup branch data
    BranchData branchData;
    const int nBranch = 5;
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

    Eigen::MatrixXcd Y = computeAdmittanceMatrix(busData, branchData);

    bool converged = GaussSeidel(Y, busData, 1024, 1E-8, 1.0);

    dispBusData(busData);
    dispLineFlow(busData, branchData, Y);

    REQUIRE(converged);

    // Voltage magnitudes
    REQUIRE(busData.V(0) == Catch::Approx(1.0000000000000000).margin(1E-12));
    REQUIRE(busData.V(1) == Catch::Approx(0.8337678171370211).margin(1E-12));
    REQUIRE(busData.V(2) == Catch::Approx(1.0500000000000000).margin(1E-12));
    REQUIRE(busData.V(3) == Catch::Approx(1.0193022826993177).margin(1E-12));
    REQUIRE(busData.V(4) == Catch::Approx(0.9742884694433818).margin(1E-12));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0) == Catch::Approx(0.0).margin(1E-12));
    REQUIRE(busData.delta(1) == Catch::Approx(-22.40641804643159).margin(1E-9));
    REQUIRE(busData.delta(2) == Catch::Approx(-0.5973464891581161).margin(1E-10));
    REQUIRE(busData.delta(3) == Catch::Approx(-2.833974239764798).margin(1E-10));
    REQUIRE(busData.delta(4) == Catch::Approx(-4.547884420849281).margin(1E-10));

    // Generator outputs
    REQUIRE(busData.Pg(0) == Catch::Approx(3.948387578413601).margin(1E-12));
    REQUIRE(busData.Qg(0) == Catch::Approx(1.142829005432617).margin(1E-12));
    REQUIRE(busData.Pg(2) == Catch::Approx(5.200000000000000).margin(1E-12));
    REQUIRE(busData.Qg(2) == Catch::Approx(3.374796297950904).margin(1E-12));

    // Qg limits for PV bus
    REQUIRE(busData.Qg(2) >= busData.Qgmin(2) - 1E-10);
    REQUIRE(busData.Qg(2) <= busData.Qgmax(2) + 1E-10);

    // Power losses
    double totalPg = busData.Pg.sum();
    double totalPl = busData.Pl.sum();
    double totalQg = busData.Qg.sum();
    double totalQl = busData.Ql.sum();

    double PLoss = totalPg - totalPl;
    double QLoss = totalQg - totalQl;

    REQUIRE(PLoss > 0.0);
    REQUIRE(QLoss > 0.0);
}
