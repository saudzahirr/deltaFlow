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

TEST_CASE("Newton-Raphson 5-Bus Test", "[Newton-Raphson][5-Bus]") {
    DEBUG("Testing [Newton-Raphson][5-Bus] - 5 Bus System Power Flow ...");

    auto busData    = create5BusBusData();
    auto branchData = create5BusBranchData();
    const int N = busData.ID.size();

    bool converged = solvePowerFlowNR(busData, branchData);
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
