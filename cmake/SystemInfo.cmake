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
#     SystemInfo.cmake
#
# Description
#     CMake configuration file that gathers and displays system information 
#     for the deltaFlow project. It provides details about the build environment, 
#     including the system name, processor type, operating system version, 
#     and CMake configuration settings.
#
# Author
#     Saud Zahir
#
# Date
#     10 December 2024
#--------------------------------------------------------------------------------

macro(SYSTEM_INFO)
    message(STATUS "System Name:: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "Host System Processor:: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    message(STATUS "System Processor:: ${CMAKE_SYSTEM_PROCESSOR}")

    message(STATUS "CMake Version:: ${CMAKE_VERSION}")
    message(STATUS "CMake Generator:: ${CMAKE_GENERATOR}")
    message(STATUS "CMake Generator Platform:: ${CMAKE_GENERATOR_PLATFORM}")
    message(STATUS "CMake Build Tool:: ${CMAKE_MAKE_PROGRAM}")
    message(STATUS "Operating System:: ${CMAKE_SYSTEM}")
    message(STATUS "Operating System Version:: ${CMAKE_SYSTEM_VERSION}")
    message(STATUS "Processor Architecture:: ${CMAKE_SYSTEM_PROCESSOR}")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "System Architecture: 64-bit")
    else()
        message(STATUS "System Architecture: 32-bit")
    endif()

endmacro()
