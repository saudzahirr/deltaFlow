#---------------------------------*- cmake -*------------------------------------
#
#                                   /\\
#                                  /  \\
#                                 /    \\
#                                /      \\
#                                =========
#
#                                deltaFlow
#                       Power System Analysis Tool
#                           Copyright (c) 2025
#--------------------------------------------------------------------------------
# License
#     This file is part of deltaFlow.
#
#    deltaFlow is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    deltaFlow is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with deltaFlow.  If not, see <https://www.gnu.org/licenses/>.
#
# File
#     CMakeLists.txt
#
# Description
#     CMake build configuration for the deltaFlow project.
#
# Author
#     Saud Zahir
#
# Date
#     09 December 2024
#--------------------------------------------------------------------------------

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
get_property(FORTRAN_SOURCES GLOBAL PROPERTY FORTRAN_SOURCES)

include_directories(${INCLUDES})

add_library(SubroutineObject OBJECT ${FORTRAN_SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES} $<TARGET_OBJECTS:SubroutineObject>)
target_link_libraries(${PROJECT_NAME} fmt)
