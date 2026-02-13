#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <complex>
#include <iostream>

#include "Admittance.H"
#include "Logger.H"
#include "Reader.H"
#include "Utils.H"


TEST_CASE("Admittance Matrix Computation - 5 Bus System", "[computeAdmittanceMatrix]") {
    DEBUG("Testing [computeAdmittanceMatrix] - 5 Bus System ...");

    BusData busData;
    BranchData branchData;

    int nBus = 5;
    busData.ID = Eigen::VectorXi(nBus);
    busData.Gs = Eigen::VectorXd(nBus);
    busData.Bs = Eigen::VectorXd(nBus);
    busData.ID << 1, 2, 3, 4, 5;
    busData.Gs.setZero();
    busData.Bs.setZero();

    int nBranch = 5;
    branchData.From     = Eigen::VectorXi(nBranch);
    branchData.To       = Eigen::VectorXi(nBranch);
    branchData.R        = Eigen::VectorXd(nBranch);
    branchData.X        = Eigen::VectorXd(nBranch);
    branchData.B        = Eigen::VectorXd(nBranch);
    branchData.tapRatio = Eigen::VectorXd(nBranch);

    branchData.From     << 1, 2, 2, 3, 4;
    branchData.To       << 5, 4, 5, 4, 5;
    branchData.R        << 0.0015, 0.009, 0.0045, 0.00075, 0.00225;
    branchData.X        << 0.020, 0.100, 0.050, 0.010, 0.025;
    branchData.B        << 0.00, 1.72, 0.88, 0.00, 0.44;
    branchData.tapRatio.setOnes();

    Eigen::MatrixXcd Y = computeAdmittanceMatrix(busData, branchData);

    DEBUG("Admittance Matrix");
    DEBUG("{}", std::string(80, '='));
    for (int i = 0; i < Y.rows(); ++i) {
        std::stringstream rowStream;
        for (int j = 0; j < Y.cols(); ++j) {
            auto R = Y(i, j).real();
            auto X = Y(i, j).imag();

            std::stringstream cell;
            if (X >= 0) {
                cell << "(" << R << " + " << X << "i)";
            } else {
                cell << "(" << R << " - " << -X << "i)";
            }

            rowStream << std::right;

            rowStream << std::setw(22) << cell.str();

            if (j != Y.cols() - 1)
                rowStream << "\t";
        }
        DEBUG("{}", rowStream.str().c_str());
    }


    // ===== Basic structure checks =====
    REQUIRE(Y.rows() == nBus);
    REQUIRE(Y.cols() == nBus);

    // Check symmetry
    for (int i = 0; i < nBus; ++i) {
        for (int j = 0; j < nBus; ++j) {
            REQUIRE(std::abs(Y(i, j) - Y(j, i)) < 1e-12);
        }
    }

    // Spot check: ensure nonzero admittances for known connections
    REQUIRE(std::abs(Y(0, 4)) > 0.0); // Bus 1 <-> 5
    REQUIRE(std::abs(Y(1, 3)) > 0.0); // Bus 2 <-> 4
    REQUIRE(std::abs(Y(1, 4)) > 0.0); // Bus 2 <-> 5
    REQUIRE(std::abs(Y(2, 3)) > 0.0); // Bus 3 <-> 4
    REQUIRE(std::abs(Y(3, 4)) > 0.0); // Bus 4 <-> 5

    // ===== Full matrix comparison =====
    Eigen::MatrixXcd Y_ref(nBus, nBus);
    Y_ref <<
        std::complex<double>( 3.72902, -49.7203), std::complex<double>(0.0, 0.0),            std::complex<double>(0.0, 0.0),            std::complex<double>(0.0, 0.0),            std::complex<double>(-3.72902, 49.7203),
        std::complex<double>(0.0, 0.0),           std::complex<double>( 2.67831, -28.459),   std::complex<double>(0.0, 0.0),            std::complex<double>(-0.892769, 9.91965),  std::complex<double>(-1.78554, 19.8393),
        std::complex<double>(0.0, 0.0),           std::complex<double>(0.0, 0.0),            std::complex<double>( 7.45805, -99.4406), std::complex<double>(-7.45805, 99.4406),   std::complex<double>(0.0, 0.0),
        std::complex<double>(0.0, 0.0),           std::complex<double>(-0.892769, 9.91965),  std::complex<double>(-7.45805, 99.4406),  std::complex<double>(11.9219, -147.959),  std::complex<double>(-3.57107, 39.6786),
        std::complex<double>(-3.72902, 49.7203),  std::complex<double>(-1.78554, 19.8393),   std::complex<double>(0.0, 0.0),            std::complex<double>(-3.57107, 39.6786),   std::complex<double>( 9.08564, -108.578);

    double tol = 1e-3;
    for (int i = 0; i < nBus; ++i) {
        for (int j = 0; j < nBus; ++j) {
            auto x = std::abs(Y(i, j).real() - Y_ref(i, j).real());
            auto y = std::abs(Y(i, j).imag() - Y_ref(i, j).imag());
            DEBUG("Mismatch at ({}, {}): {} + {}i", i, j, x, y);
            REQUIRE(x < tol);
            REQUIRE(y < tol);
        }
    }
}
