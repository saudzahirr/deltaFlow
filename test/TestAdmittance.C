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

#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <complex>
#include <iomanip>
#include <sstream>

#include "Admittance.H"
#include "Logger.H"
#include "TestUtils.H"


TEST_CASE("Admittance Matrix Computation - 5 Bus System", "[Admittance][5-Bus]") {
    DEBUG("Testing [Admittance][5-Bus] - Admittance Matrix Computation ...");

    BusData busData;
    const int nBus = 5;
    busData.ID = Eigen::VectorXi(nBus);
    busData.Gs = Eigen::VectorXd(nBus);
    busData.Bs = Eigen::VectorXd(nBus);
    busData.ID << 1, 2, 3, 4, 5;
    busData.Gs.setZero();
    busData.Bs.setZero();

    auto branchData = create5BusBranchData();

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
