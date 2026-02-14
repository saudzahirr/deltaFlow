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

#include "IEEE.H"
#include "Logger.H"
#include "TestUtils.H"

TEST_CASE("IEEE CDF 300-Bus Power Flow", "[IEEE][CDF][300-Bus]") {
    LOG_DEBUG("Testing [IEEE][CDF][300-Bus] - IEEE CDF 300-Bus Power Flow ...");

    IEEECommonDataFormat reader;
    reader.read(testDataDir("IEEE") + "IEEE300.txt");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 300);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Total generation matches expected
    REQUIRE(busData.Pg.sum() == Catch::Approx(232.0044).margin(0.5));
    REQUIRE(busData.Qg.sum() == Catch::Approx(84.4283).margin(1.0));

    // All voltages within reasonable range
    for (int i = 0; i < N; ++i) {
        REQUIRE(busData.V(i) > 0.8);
        REQUIRE(busData.V(i) < 1.2);
    }
}
