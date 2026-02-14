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

TEST_CASE("IEEE CDF 30-Bus Power Flow", "[IEEE][CDF][30-Bus]") {
    LOG_DEBUG("Testing [IEEE][CDF][30-Bus] - IEEE CDF 30-Bus Power Flow ...");

    IEEECommonDataFormat reader;
    reader.read(testDataDir("IEEE") + "IEEE30.txt");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 30);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes
    REQUIRE(busData.V(0)  == Catch::Approx(1.0600).margin(1e-4));
    REQUIRE(busData.V(1)  == Catch::Approx(1.0430).margin(1e-4));
    REQUIRE(busData.V(4)  == Catch::Approx(1.0100).margin(1e-4));
    REQUIRE(busData.V(7)  == Catch::Approx(1.0100).margin(1e-4));
    REQUIRE(busData.V(10) == Catch::Approx(1.0820).margin(1e-4));
    REQUIRE(busData.V(12) == Catch::Approx(1.0710).margin(1e-4));
    REQUIRE(busData.V(29) == Catch::Approx(0.9919).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(1)  == Catch::Approx(-5.3500).margin(1e-4));
    REQUIRE(busData.delta(4)  == Catch::Approx(-14.1671).margin(1e-4));
    REQUIRE(busData.delta(29) == Catch::Approx(-17.6562).margin(1e-4));

    // Slack bus power (Bus 1)
    REQUIRE(busData.Pg(0) == Catch::Approx(2.6095).margin(1e-4));
    REQUIRE(busData.Qg(0) == Catch::Approx(-0.1653).margin(1e-4));

    // PV bus reactive power
    REQUIRE(busData.Qg(1)  == Catch::Approx(0.4956).margin(1e-4));
    REQUIRE(busData.Qg(4)  == Catch::Approx(0.3694).margin(1e-4));
    REQUIRE(busData.Qg(7)  == Catch::Approx(0.3722).margin(1e-4));
    REQUIRE(busData.Qg(10) == Catch::Approx(0.1618).margin(1e-4));
    REQUIRE(busData.Qg(12) == Catch::Approx(0.1063).margin(1e-4));

    // Power losses
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    REQUIRE(PLoss == Catch::Approx(0.1755).margin(1e-3));
    REQUIRE(PLoss > 0.0);
}
