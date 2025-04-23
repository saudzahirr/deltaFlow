#---------------------------------*- cmake -*-------------------------------------
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
#     RegisterGlobalSourcesAndIncludes.cmake
#
# Description
#     CMake configuration file that registers and manages global properties 
#     for source files and include directories used in the deltaFlow project.
#     The file includes the macro `REGISTER_GLOBAL_SOURCES_AND_INCLUDES`, 
#     which performs the following tasks:
#     - Collects and registers all source files with a `.C` extension in 
#       the current directory to the global `SOURCES` property.
#     - Registers the current source and binary directories as include 
#       directories for the project.
#
#     This ensures that the source files and header paths are globally 
#     accessible throughout the build process.
#
# Author
#     Saud Zahir
#
# Date
#     10 December 2024
#--------------------------------------------------------------------------------

macro(REGISTER_GLOBAL_SOURCES_AND_INCLUDES VARIABLE_SOURCES VARIABLE_INCLUDES)
    file(GLOB SOURCES *.C)

    get_property(CURRENT_SOURCES GLOBAL PROPERTY ${VARIABLE_SOURCES})
    set_property(GLOBAL PROPERTY ${VARIABLE_SOURCES} "${CURRENT_SOURCES}" "${SOURCES}")

    set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}")

    get_property(CURRENT_INCLUDES GLOBAL PROPERTY ${VARIABLE_INCLUDES})
    set_property(GLOBAL PROPERTY ${VARIABLE_INCLUDES} "${CURRENT_INCLUDES}" "${INCLUDE_DIRS}")
endmacro()
