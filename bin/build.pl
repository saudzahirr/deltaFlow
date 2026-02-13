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

open(my $fh, '<', 'version') or die "Cannot open version file: $!\n";
chomp(my $version = <$fh>);
close($fh);

if ($args->{build} || $args->{test}) {
    system("conan install . --output-folder=build --build=missing -s compiler.cppstd=17") == 0
        or die "Conan install failed\n";
}

if ($args->{build}) {
    print "Running build (without tests)...\n";

    system("cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=OFF -DPROJECT_VERSION=$version") == 0
        or die "CMake configuration failed\n";

    system("cmake --build build --config Release") == 0 or die "Build failed\n";
}

if ($args->{test}) {
    print "Running tests...\n";

    system("cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DPROJECT_VERSION=$version") == 0
        or die "CMake configuration failed\n";

    system("cmake --build build --config Release") == 0 or die "Build failed\n";
    system("ctest --output-on-failure --test-dir build --build-config Release") == 0 or die "Tests failed\n";
}

if ($args->{doc}) {
    print "Building docs...\n";

    system("doxygen Doxyfile") == 0
        or die "Doxyfile configuration failed\n";
}
