#!/bin/perl

use strict;
use warnings;

sub print_help {
    print <<"END_HELP";
Usage: $0 [OPTIONS]

Options:
  --build, -b     Run the build process (without tests)
  --test,  -t     Run the build process with tests
  --doc,   -d     Generate documentation
  --help,  -h     Show this help message and exit
END_HELP
    exit;
}

sub parse_args {
    my %args = (
        build => 0,
        test  => 0,
        doc   => 0,
    );

    if (!@ARGV) {
        print_help();
    }

    while (@ARGV) {
        my $arg = shift @ARGV;
        if ($arg eq '--build' || $arg eq '-b') {
            $args{build} = 1;
        } elsif ($arg eq '--test' || $arg eq '-t') {
            $args{test} = 1;
        } elsif ($arg eq '--doc' || $arg eq '-d') {
            $args{doc} = 1;
        } elsif ($arg eq '--help' || $arg eq '-h') {
            print_help();
        } else {
            die "Unknown option: $arg\nUse --help to see available options.\n";
        }
    }

    return \%args;
}

my $args = parse_args();

if ($args->{build}) {
    print "Running build (without tests)...\n";

    system("cmake -S . -B build -DBUILD_TEST=OFF") == 0
        or die "CMake configuration failed\n";

    chdir "build" or die "Cannot change directory to build\n";
    system("make") == 0 or die "Build failed\n";
    chdir ".." or die "Cannot change back to project root\n";
}

if ($args->{test}) {
    print "Running tests...\n";

    system("cmake -S . -B build -DBUILD_TEST=ON") == 0
        or die "CMake configuration failed\n";

    chdir "build" or die "Cannot change directory to build\n";
    system("make") == 0 or die "Build failed\n";
    chdir ".." or die "Cannot change back to project root\n";
}

if ($args->{doc}) {
    print "Building docs...\n";

    system("doxygen Doxyfile") == 0
        or die "Doxyfile configuration failed\n";
}
