#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <cmath>
#include <cassert>
#include "types.H"

// Element-wise arithmetic for real vectors
DoubleVector operator+(const DoubleVector& a, const DoubleVector& b);
DoubleVector operator-(const DoubleVector& a, const DoubleVector& b);
DoubleVector operator*(const DoubleVector& a, const DoubleVector& b);
DoubleVector operator*(double scalar, const DoubleVector& vec);

// Trigonometric functions
DoubleVector cos(const DoubleVector& vec);
DoubleVector sin(const DoubleVector& vec);

// Overloads for complex arithmetic
ComplexVector operator*(std::complex<double> scalar, const DoubleVector& realVec);
ComplexVector operator+(const DoubleVector& realVec, const ComplexVector& complexVec);
ComplexVector operator*(const DoubleVector& realVec, const ComplexVector& complexVec);

#endif
