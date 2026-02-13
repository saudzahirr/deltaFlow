#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "Logger.H"
#include "TestUtils.H"

TEST_CASE("Newton-Raphson Divergence Test", "[Newton-Raphson][Divergence]") {
    DEBUG("Testing [Newton-Raphson][Divergence] - Expected failure case ...");

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
    DEBUG("Testing [Gauss-Seidel][Divergence] - Expected failure case ...");

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
