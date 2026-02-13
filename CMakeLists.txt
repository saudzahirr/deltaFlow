cmake_minimum_required(VERSION 3.25)
project(deltaFlow LANGUAGES CXX VERSION ${PROJECT_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include Conan-generated files
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB CMAKE_FILES "${CMAKE_SOURCE_DIR}/cmake/*.cmake")

foreach(CMAKE_FILE ${CMAKE_FILES})
    include(${CMAKE_FILE})
endforeach()

find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)

SYSTEM_INFO()

set(EXECUTABLE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

add_subdirectory(src)

configure_file(
  "${PROJECT_SOURCE_DIR}/src/version.h.in"
  "${PROJECT_SOURCE_DIR}/src/Version.H"
)

option(BUILD_TEST "Build tests" OFF)

if(BUILD_TEST)
    enable_testing()
    find_package(Catch2 REQUIRED)
    add_subdirectory(test)
endif()
