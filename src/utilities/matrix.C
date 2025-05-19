#include "matrix.H"

// Real vector arithmetic
DoubleVector operator+(const DoubleVector& a, const DoubleVector& b) {
    assert(a.size() == b.size());
    DoubleVector result(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        result[i] = a[i] + b[i];
    return result;
}

DoubleVector operator-(const DoubleVector& a, const DoubleVector& b) {
    assert(a.size() == b.size());
    DoubleVector result(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        result[i] = a[i] - b[i];
    return result;
}

DoubleVector operator*(const DoubleVector& a, const DoubleVector& b) {
    assert(a.size() == b.size());
    DoubleVector result(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        result[i] = a[i] * b[i];
    return result;
}

DoubleVector operator*(double scalar, const DoubleVector& vec) {
    DoubleVector result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
        result[i] = scalar * vec[i];
    return result;
}

// Trigonometric functions
DoubleVector cos(const DoubleVector& vec) {
    DoubleVector result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
        result[i] = std::cos(vec[i]);
    return result;
}

DoubleVector sin(const DoubleVector& vec) {
    DoubleVector result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
        result[i] = std::sin(vec[i]);
    return result;
}

// Complex operations
ComplexVector operator*(std::complex<double> scalar, const DoubleVector& realVec) {
    ComplexVector result(realVec.size());
    for (size_t i = 0; i < realVec.size(); ++i)
        result[i] = scalar * realVec[i];
    return result;
}

ComplexVector operator+(const DoubleVector& realVec, const ComplexVector& complexVec) {
    assert(realVec.size() == complexVec.size());
    ComplexVector result(realVec.size());
    for (size_t i = 0; i < realVec.size(); ++i)
        result[i] = std::complex<double>(realVec[i]) + complexVec[i];
    return result;
}

ComplexVector operator*(const DoubleVector& realVec, const ComplexVector& complexVec) {
    assert(realVec.size() == complexVec.size());
    ComplexVector result(realVec.size());
    for (size_t i = 0; i < realVec.size(); ++i)
        result[i] = realVec[i] * complexVec[i];
    return result;
}
