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

#include "Logger.H"
#include "TestUtils.H"

TEST_CASE("Newton-Raphson Divergence Test", "[Newton-Raphson][Divergence]") {
    LOG_DEBUG("Testing [Newton-Raphson][Divergence] - Expected failure case ...");

    // Use the standard 5-bus system but with extreme load to cause divergence
    auto busData    = create5BusBusData();
    auto branchData = create5BusBranchData();

    // Push the load far beyond what the network can handle
    busData.Pl(1) = 800.0;   // 100x normal load
    busData.Ql(1) = 280.0;
    busData.Pl(3) = 500.0;
    busData.Ql(3) = 200.0;

    // Allow only a few iterations so the red bar is clearly visible
    bool converged = solvePowerFlowNR(busData, branchData, 10, 1E-8);
    REQUIRE_FALSE(converged);
}

TEST_CASE("Gauss-Seidel Divergence Test", "[Gauss-Seidel][Divergence]") {
    LOG_DEBUG("Testing [Gauss-Seidel][Divergence] - Expected failure case ...");

    auto busData    = create5BusBusData();
    auto branchData = create5BusBranchData();

    // Extreme load
    busData.Pl(1) = 800.0;
    busData.Ql(1) = 280.0;
    busData.Pl(3) = 500.0;
    busData.Ql(3) = 200.0;

    bool converged = solvePowerFlowGS(busData, branchData, 10, 1E-8, 1.0);
    REQUIRE_FALSE(converged);
}
