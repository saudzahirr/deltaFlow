#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "IEEE.H"
#include "Logger.H"
#include "TestUtils.H"

TEST_CASE("IEEE CDF 14-Bus Power Flow", "[IEEE][CDF][14-Bus]") {
    DEBUG("Testing [IEEE][CDF][14-Bus] - IEEE CDF 14-Bus Power Flow ...");

    IEEECommonDataFormat reader;
    reader.read(testDataDir("IEEE") + "IEEE14.txt");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 14);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes
    REQUIRE(busData.V(0)  == Catch::Approx(1.0600).margin(1e-4));
    REQUIRE(busData.V(1)  == Catch::Approx(1.0450).margin(1e-4));
    REQUIRE(busData.V(2)  == Catch::Approx(1.0100).margin(1e-4));
    REQUIRE(busData.V(3)  == Catch::Approx(1.0177).margin(1e-4));
    REQUIRE(busData.V(4)  == Catch::Approx(1.0195).margin(1e-4));
    REQUIRE(busData.V(5)  == Catch::Approx(1.0700).margin(1e-4));
    REQUIRE(busData.V(7)  == Catch::Approx(1.0900).margin(1e-4));
    REQUIRE(busData.V(13) == Catch::Approx(1.0355).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(1)  == Catch::Approx(-4.9826).margin(1e-4));
    REQUIRE(busData.delta(2)  == Catch::Approx(-12.7251).margin(1e-4));
    REQUIRE(busData.delta(5)  == Catch::Approx(-14.2209).margin(1e-4));
    REQUIRE(busData.delta(13) == Catch::Approx(-16.0336).margin(1e-4));

    // Slack bus power (Bus 1)
    REQUIRE(busData.Pg(0) == Catch::Approx(2.3239).margin(1e-4));
    REQUIRE(busData.Qg(0) == Catch::Approx(-0.1655).margin(1e-4));

    // PV bus reactive power (Bus 2, 3, 6, 8)
    REQUIRE(busData.Qg(1) == Catch::Approx(0.4356).margin(1e-4));
    REQUIRE(busData.Qg(2) == Catch::Approx(0.2508).margin(1e-4));
    REQUIRE(busData.Qg(5) == Catch::Approx(0.1273).margin(1e-4));
    REQUIRE(busData.Qg(7) == Catch::Approx(0.1762).margin(1e-4));

    // Power losses
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    double QLoss = busData.Qg.sum() - busData.Ql.sum();
    REQUIRE(PLoss == Catch::Approx(0.1339).margin(1e-3));
    REQUIRE(PLoss > 0.0);
}
