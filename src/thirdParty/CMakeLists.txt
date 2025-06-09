#--------------------------------------------------------------------------------#
add_library(fmt STATIC
    fmt-10.2.1/src/format.cc
)
target_include_directories(fmt PUBLIC fmt-10.2.1/include)
add_library(Eigen3 INTERFACE)
target_include_directories(Eigen3 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Eigen_3.4.0)
#--------------------------------------------------------------------------------#
