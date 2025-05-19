file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(SUBDIR ${SUBDIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR})
        add_subdirectory(${SUBDIR})
    endif()
endforeach()

#--------------------------------------------------------------------------------#
REGISTER_GLOBAL_SOURCES_AND_INCLUDES(SOURCES INCLUDES)
#--------------------------------------------------------------------------------#

get_property(SOURCES GLOBAL PROPERTY SOURCES)
get_property(INCLUDES GLOBAL PROPERTY INCLUDES)

include_directories(${INCLUDES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} fmt Eigen3)

target_include_directories(${PROJECT_NAME} PRIVATE ${EIGEN3_INCLUDE_DIRS} .)
