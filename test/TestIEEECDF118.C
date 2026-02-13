#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "IEEE.H"
#include "Logger.H"
#include "TestUtils.H"

TEST_CASE("IEEE CDF 118-Bus Power Flow", "[IEEE][CDF][118-Bus]") {
    DEBUG("Testing [IEEE][CDF][118-Bus] - IEEE CDF 118-Bus Power Flow ...");

    IEEECommonDataFormat reader;
    reader.read(testDataDir("IEEE") + "IEEE118.txt");

    auto busData    = reader.getBusData();
    auto branchData = reader.getBranchData();

    int N = busData.ID.size();
    REQUIRE(N == 118);

    bool converged = solvePowerFlowNR(busData, branchData);
    REQUIRE(converged);

    // Voltage magnitudes (spot checks)
    REQUIRE(busData.V(0)  == Catch::Approx(0.9550).margin(1e-4));
    REQUIRE(busData.V(68) == Catch::Approx(1.0350).margin(1e-4));
    REQUIRE(busData.V(88) == Catch::Approx(1.0050).margin(1e-4));

    // Voltage angles (degrees)
    REQUIRE(busData.delta(0)  == Catch::Approx(0.0).margin(1e-4));
    REQUIRE(busData.delta(68) == Catch::Approx(19.3834).margin(1e-3));
    REQUIRE(busData.delta(88) == Catch::Approx(29.1001).margin(1e-3));

    // Generator outputs (Bus 69 and Bus 89 are major generators)
    REQUIRE(busData.Pg(68) == Catch::Approx(5.1640).margin(1e-3));
    REQUIRE(busData.Pg(88) == Catch::Approx(6.0700).margin(1e-3));

    // Total generation and load balance
    double totalPg = busData.Pg.sum();
    double totalPl = busData.Pl.sum();
    double PLoss = totalPg - totalPl;
    REQUIRE(PLoss == Catch::Approx(1.354).margin(0.1));
    REQUIRE(PLoss > 0.0);
}
