get_property(SOURCES GLOBAL PROPERTY SOURCES)
get_property(INCLUDES GLOBAL PROPERTY INCLUDES)

# Exclude main.C from test sources
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.C$")

add_library(deltaFlowLib STATIC ${SOURCES})
target_include_directories(deltaFlowLib PUBLIC ${INCLUDES})
target_link_libraries(deltaFlowLib PUBLIC fmt::fmt Eigen3::Eigen)

# Catch2 test dependencies
set(TEST_LIBS Catch2::Catch2WithMain deltaFlowLib)

# Add TestAdmittance executable
add_executable(TestAdmittance TestAdmittance.C ${SOURCES})
target_link_libraries(TestAdmittance PRIVATE ${TEST_LIBS})
target_include_directories(TestAdmittance PRIVATE ${EIGEN3_INCLUDE_DIRS} .)

# Add TestGaussSeidel executable
add_executable(TestGaussSeidel TestGaussSeidel.C ${SOURCES})
target_link_libraries(TestGaussSeidel PRIVATE ${TEST_LIBS})
target_include_directories(TestGaussSeidel PRIVATE ${EIGEN3_INCLUDE_DIRS} .)

# Add TestNewtonRaphson executable
add_executable(TestNewtonRaphson TestNewtonRaphson.C ${SOURCES})
target_link_libraries(TestNewtonRaphson PRIVATE ${TEST_LIBS})
target_include_directories(TestNewtonRaphson PRIVATE ${EIGEN3_INCLUDE_DIRS} .)

# Enable automatic Catch2 test discovery
include(Catch)
catch_discover_tests(TestAdmittance)
catch_discover_tests(TestGaussSeidel)
catch_discover_tests(TestNewtonRaphson)
