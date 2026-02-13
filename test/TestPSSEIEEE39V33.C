#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "Logger.H"
#include "PSSE.H"
#include "TestUtils.H"

TEST_CASE("PSS/E v33 IEEE 39-Bus Power Flow", "[PSSE][IEEE][39-Bus][v33]") {
    DEBUG("Testing [PSSE][IEEE][39-Bus][v33] - PSS/E v33 IEEE 39-Bus Power Flow ...");

    PSSERawFormat reader;
    reader.read(testDataDir("PSSE") + "IEEE39_v33.raw");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 39);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes (spot checks)
    REQUIRE(busData.V(0)  == Catch::Approx(1.0095).margin(1e-4));
    REQUIRE(busData.V(29) == Catch::Approx(0.9658).margin(1e-4));
    REQUIRE(busData.V(30) == Catch::Approx(0.7126).margin(1e-4));
    REQUIRE(busData.V(31) == Catch::Approx(0.8752).margin(1e-4));
    REQUIRE(busData.V(32) == Catch::Approx(0.9972).margin(1e-4));
    REQUIRE(busData.V(38) == Catch::Approx(1.0200).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(29) == Catch::Approx(-0.3889).margin(1e-3));
    REQUIRE(busData.delta(30) == Catch::Approx(5.0975).margin(1e-3));
    REQUIRE(busData.delta(38) == Catch::Approx(1.5534).margin(1e-3));

    // Generator outputs (Bus 30, 31, 32, 33, 39)
    REQUIRE(busData.Pg(29) == Catch::Approx(2.2400).margin(1e-3));
    REQUIRE(busData.Qg(29) == Catch::Approx(1.5318).margin(1e-3));
    REQUIRE(busData.Pg(30) == Catch::Approx(5.7293).margin(1e-3));
    REQUIRE(busData.Pg(31) == Catch::Approx(6.5000).margin(1e-3));
    REQUIRE(busData.Pg(32) == Catch::Approx(6.3200).margin(1e-3));
    REQUIRE(busData.Pg(38) == Catch::Approx(7.4998).margin(1e-3));
    REQUIRE(busData.Qg(38) == Catch::Approx(5.9026).margin(1e-2));

    // Power losses
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    REQUIRE(PLoss == Catch::Approx(0.6011).margin(0.05));
    REQUIRE(PLoss > 0.0);
}
