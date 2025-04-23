#ifndef TYPES_H
#define TYPES_H

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

// #ifdef __cplusplus
//     #ifdef __GNUC__
//         typedef __complex__ float COMPLEX;
//         typedef __complex__ double DOUBLE_COMPLEX;
//     #else
//         #include <complex>
//         typedef std::complex<float> COMPLEX;
//         typedef std::complex<double> DOUBLE_COMPLEX;
//     #endif
// #else
//     typedef _Complex float COMPLEX;
//     typedef _Complex double DOUBLE_COMPLEX;
// #endif

#include <complex>
typedef std::complex<float> COMPLEX;
typedef std::complex<double> DOUBLE_COMPLEX;

#endif
