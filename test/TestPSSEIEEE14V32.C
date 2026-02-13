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
#include "PSSE.H"
#include "TestUtils.H"

TEST_CASE("PSS/E v32 IEEE 14-Bus Power Flow", "[PSSE][IEEE][14-Bus][v32]") {
    DEBUG("Testing [PSSE][IEEE][14-Bus][v32] - PSS/E v32 IEEE 14-Bus Power Flow ...");

    PSSERawFormat reader;
    reader.read(testDataDir("PSSE") + "IEEE14_v32.raw");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 14);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes
    REQUIRE(busData.V(0)  == Catch::Approx(1.0300).margin(1e-4));
    REQUIRE(busData.V(1)  == Catch::Approx(1.0047).margin(1e-4));
    REQUIRE(busData.V(2)  == Catch::Approx(0.9746).margin(1e-4));
    REQUIRE(busData.V(3)  == Catch::Approx(0.9640).margin(1e-4));
    REQUIRE(busData.V(5)  == Catch::Approx(0.9350).margin(1e-4));
    REQUIRE(busData.V(7)  == Catch::Approx(0.9526).margin(1e-4));
    REQUIRE(busData.V(13) == Catch::Approx(0.8852).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(1)  == Catch::Approx(-1.3893).margin(1e-4));
    REQUIRE(busData.delta(2)  == Catch::Approx(-3.1719).margin(1e-4));
    REQUIRE(busData.delta(5)  == Catch::Approx(-6.4228).margin(1e-4));
    REQUIRE(busData.delta(13) == Catch::Approx(-9.2289).margin(1e-4));

    // Slack bus power (Bus 1)
    REQUIRE(busData.Pg(0) == Catch::Approx(0.8230).margin(1e-4));
    REQUIRE(busData.Qg(0) == Catch::Approx(0.4163).margin(1e-4));

    // Generator outputs
    REQUIRE(busData.Pg(1) == Catch::Approx(0.4000).margin(1e-4));
    REQUIRE(busData.Qg(1) == Catch::Approx(0.1500).margin(1e-4));
    REQUIRE(busData.Pg(2) == Catch::Approx(0.4000).margin(1e-4));
    REQUIRE(busData.Pg(5) == Catch::Approx(0.3000).margin(1e-4));
    REQUIRE(busData.Pg(7) == Catch::Approx(0.3500).margin(1e-4));

    // Power losses
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    REQUIRE(PLoss == Catch::Approx(0.0360).margin(1e-3));
    REQUIRE(PLoss > 0.0);
}
