#!/usr/bin/perl

#---------------------------------*- perl -*-------------------------------------
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
# Script
#     build.pl
#
# Description
#     Build script for deltaFlow project. This script ensures the build directory is
#     created, runs CMake to configure the project, and compiles it using make.
#
# Author
#     Saud Zahir
#
# Date
#     09 December 2024
#--------------------------------------------------------------------------------

use strict;
use warnings;
use File::Path qw(make_path);
use Cwd;

my $build_dir = "build";

unless (-d $build_dir) {
    print "Creating build directory: $build_dir\n";
    make_path($build_dir) or die "Failed to create build directory: $!\n";
}

chdir($build_dir) or die "Failed to change directory to $build_dir: $!\n";
system("cmake --version && cmake ..") == 0 or die "CMake configuration failed: $!\n";
system("make --version && make") == 0 or die "Make build failed: $!\n";
