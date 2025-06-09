#!/bin/perl

use strict;
use warnings;

system("cmake -S . -B build") == 0 or die "CMake configuration failed\n";

chdir "build" or die "Cannot change directory to build\n";
system("make") == 0 or die "Build failed\n";
chdir ".." or die "Cannot change back to project root\n";
