#!/bin/perl

use strict;
use warnings;

my @tests = (
    './bin/TestAdmittance',
    './bin/TestGaussSeidel',
    './bin/TestNewtonRaphson'
);

print "Running all test suites...\n\n";

foreach my $test (@tests) {
    print "Running $test...\n";
    my $rv = system($test);

    if ($rv == 0) {
        print "[PASS] $test\n";
    }
    else {
        my $exit_code = $rv >> 8;
        print "[FAIL] $test (exit code $exit_code)\n";
    }

    print "-" x 80 . "\n";
}

print "All tests complete.\n";
