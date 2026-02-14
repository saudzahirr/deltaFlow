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

TEST_CASE("IEEE CDF 57-Bus Power Flow", "[IEEE][CDF][57-Bus]") {
    DEBUG("Testing [IEEE][CDF][57-Bus] - IEEE CDF 57-Bus Power Flow ...");

    IEEECommonDataFormat reader;
    reader.read(testDataDir("IEEE") + "IEEE57.txt");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 57);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes (Slack and PV buses)
    REQUIRE(busData.V(0)  == Catch::Approx(1.0400).margin(1e-4));
    REQUIRE(busData.V(1)  == Catch::Approx(1.0100).margin(1e-4));
    REQUIRE(busData.V(2)  == Catch::Approx(0.9850).margin(1e-4));
    REQUIRE(busData.V(5)  == Catch::Approx(0.9800).margin(1e-4));
    REQUIRE(busData.V(7)  == Catch::Approx(1.0050).margin(1e-4));
    REQUIRE(busData.V(8)  == Catch::Approx(0.9800).margin(1e-4));
    REQUIRE(busData.V(11) == Catch::Approx(1.0150).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(1)  == Catch::Approx(-1.1882).margin(1e-4));
    REQUIRE(busData.delta(7)  == Catch::Approx(-4.4779).margin(1e-4));
    REQUIRE(busData.delta(11) == Catch::Approx(-10.4712).margin(1e-4));

    // Slack bus power (Bus 1)
    REQUIRE(busData.Pg(0) == Catch::Approx(4.7866).margin(1e-3));
    REQUIRE(busData.Qg(0) == Catch::Approx(1.2885).margin(1e-3));

    // PV bus generation (Bus 8, Bus 12)
    REQUIRE(busData.Pg(7)  == Catch::Approx(4.5000).margin(1e-4));
    REQUIRE(busData.Qg(7)  == Catch::Approx(0.6210).margin(1e-3));
    REQUIRE(busData.Pg(11) == Catch::Approx(3.1000).margin(1e-4));
    REQUIRE(busData.Qg(11) == Catch::Approx(1.2863).margin(1e-3));

    // Power losses
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    REQUIRE(PLoss == Catch::Approx(0.2786).margin(1e-2));
    REQUIRE(PLoss > 0.0);
}
