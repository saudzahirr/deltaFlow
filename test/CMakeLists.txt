get_property(SOURCES GLOBAL PROPERTY SOURCES)
get_property(INCLUDES GLOBAL PROPERTY INCLUDES)

# Exclude main.C from test sources
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.C$")

add_library(deltaFlowLib STATIC ${SOURCES})
target_include_directories(deltaFlowLib PUBLIC ${INCLUDES})
target_link_libraries(deltaFlowLib PUBLIC fmt::fmt Eigen3::Eigen)

# Catch2 test dependencies
set(TEST_LIBS Catch2::Catch2WithMain deltaFlowLib)

# Helper macro to add a test executable
macro(ADD_DELTAFLOW_TEST TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.C)
    target_link_libraries(${TEST_NAME} PRIVATE ${TEST_LIBS})
    target_include_directories(${TEST_NAME} PRIVATE ${EIGEN3_INCLUDE_DIRS} .)
    catch_discover_tests(${TEST_NAME})
endmacro()

# Enable automatic Catch2 test discovery
include(Catch)

# Solver tests
ADD_DELTAFLOW_TEST(TestAdmittance)
ADD_DELTAFLOW_TEST(TestGaussSeidel)
ADD_DELTAFLOW_TEST(TestNewtonRaphson)

# IEEE CDF reader tests
ADD_DELTAFLOW_TEST(TestIEEECDF14)
ADD_DELTAFLOW_TEST(TestIEEECDF30)
ADD_DELTAFLOW_TEST(TestIEEECDF57)
ADD_DELTAFLOW_TEST(TestIEEECDF118)
ADD_DELTAFLOW_TEST(TestIEEECDF300)

# PSS/E reader tests
ADD_DELTAFLOW_TEST(TestPSSEIEEE14V32)
ADD_DELTAFLOW_TEST(TestPSSEIEEE39V33)

# Divergence tests
ADD_DELTAFLOW_TEST(TestDivergence)
