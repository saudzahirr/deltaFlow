#ifndef TYPES_H
#define TYPES_H

#include <complex>
#include <vector>

/**
 * Fortran 77	        C/C++
 * INTEGER	            int
 * REAL	                float
 * DOUBLE PRECISION	    double
 * COMPLEX	            float _Complex
 * DOUBLE COMPLEX	    double _Complex
 */

typedef int INTEGER;
typedef float REAL;
typedef double DOUBLE;
typedef std::complex<float> COMPLEX;
typedef std::complex<double> DOUBLE_COMPLEX;

const COMPLEX I(0, 1);

using IntegerVector = std::vector<int>;
using DoubleVector = std::vector<double>;
using ComplexVector = std::vector<std::complex<double>>;

#endif
