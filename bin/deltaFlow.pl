#!/bin/perl

#---------------------------------*- perl -*-------------------------------------
#
#                                   /\\
#                                  /  \\
#                                 /    \\
#                                /      \\
#                                =========
#
#                                deltaFlow
#                        Power System Analysis Tool
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
# Script
#     deltaFlow
#
# Description
#     Perl script to execute deltaFlow MATLAB script in no-display mode.
#
# Author
#     Saud Zahir
#
# Date
#     09 December 2024
#--------------------------------------------------------------------------------


system("matlab -nodisplay -r \"run('Src/main.m'); exit;\"") == 0
    or die "Failed to execute MATLAB script: $!";
