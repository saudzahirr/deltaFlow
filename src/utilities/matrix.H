#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>
#include <cstring>
#include <complex>
#include <iostream>
#include "logger.H"

enum class Order {
    ROW_MAJOR,
    COL_MAJOR
};


template<typename T>
class Matrix final {
    public:
        Matrix(size_t rows, size_t cols, Order order = Order::ROW_MAJOR)
            : m_Rows(rows), m_Cols(cols), m_Order(order) {
            m_Data = new T[rows * cols]();
        }

        // Copy constructor
        Matrix(const Matrix<T>& other) : m_Rows(other.m_Rows), m_Cols(other.m_Cols), m_Order(other.m_Order) {
            m_Data = new T[m_Rows * m_Cols];
            std::memcpy(m_Data, other.m_Data, m_Rows * m_Cols * sizeof(T));
        }

        // Copy assignment operator
        Matrix<T>& operator=(const Matrix<T>& other) {
            if (this != &other) {
                delete[] m_Data;
                m_Rows = other.m_Rows;
                m_Cols = other.m_Cols;
                m_Order = other.m_Order;
                m_Data = new T[m_Rows * m_Cols];
                std::memcpy(m_Data, other.m_Data, m_Rows * m_Cols * sizeof(T));
            }
            return *this;
        }

        // Destructor
        ~Matrix() {
            DEBUG("Deleting matrix object ...");
            delete[] m_Data;
        }

        // Access element (mutable)
        T& operator()(size_t row, size_t col) {
            assert(row < m_Rows && col < m_Cols);
            return m_Order == Order::ROW_MAJOR
                ? m_Data[row * m_Cols + col]
                : m_Data[col * m_Rows + row];
        }

        // Access element (const)
        const T& operator()(size_t row, size_t col) const {
            assert(row < m_Rows && col < m_Cols);
            return m_Order == Order::ROW_MAJOR
                ? m_Data[row * m_Cols + col]
                : m_Data[col * m_Rows + row];
        }

        template<typename U>
        operator Matrix<U>() const {
            Matrix<U> C(m_Rows, m_Cols, m_Order);
            for (size_t i = 0; i < m_Rows; ++i)
            for (size_t j = 0; j < m_Cols; ++j)
                C(i, j) = static_cast<U>((*this)(i, j));
            return C;
        }


        // Mathematical operations
        template<typename U>
        friend Matrix<U> operator-(const Matrix<U>&, const Matrix<U>&);
        template<typename U>
        friend Matrix<U> operator+(const Matrix<U>&, const Matrix<U>&);
        template<typename U>
        friend Matrix<std::complex<U>> operator-(const Matrix<U>&, const Matrix<std::complex<U>>&);
        template<typename U>
        friend Matrix<std::complex<U>> operator+(const Matrix<U>&, const Matrix<std::complex<U>>&);

        template<typename U, typename V>
        friend Matrix<U> operator*(const U&, const Matrix<V>&);

        template<typename U>
        friend Matrix<U> operator*(const Matrix<U>&, const Matrix<U>&);
        template<typename U, typename V>
        friend Matrix<std::complex<V>> operator*(const Matrix<U>&, const Matrix<std::complex<V>>&);

        template<typename U>
        friend Matrix<U> cos(const Matrix<U>&);
        template<typename U>
        friend Matrix<U> sin(const Matrix<U>&);
        template<typename U>
        friend Matrix<U> abs(const Matrix<U>&);
        template<typename U>
        friend Matrix<U> abs(const Matrix<std::complex<U>>&);
        template<typename U>
        friend Matrix<U> angle(const Matrix<std::complex<U>>&);

        std::string str() const;

        // Get number of rows
        size_t rows() const { return m_Rows; }

        // Get number of columns
        size_t cols() const { return m_Cols; }

        T* data() {
            return m_Data;
        }

        const T* data() const {
            return m_Data;
        }

        T* begin() {
            return m_Data;
        }

        T* end() {
            return m_Data + m_Rows * m_Cols;
        }

        const T* begin() const {
            return m_Data;
        }

        const T* end() const {
            return m_Data + m_Rows * m_Cols;
        }

    private:
        size_t m_Rows;
        size_t m_Cols;
        Order m_Order;
        T* m_Data;
};


//------------------------------------------------------------------------------
// Stream‐insertion operator for Matrix<T>
template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& M) {
    for (size_t i = 0; i < M.rows(); ++i) {
        for (size_t j = 0; j < M.cols(); ++j) {
            os << M(i,j);
            if (j + 1 < M.cols()) os << "\t";
        }
        if (i + 1 < M.rows()) os << "\n";
    }
    return os;
}

template<typename T>
std::string Matrix<T>::str() const {
    std::stringstream ss;
    if (rows() == 0 || cols() == 0) {
        return "Empty matrix";
    }

    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < cols(); ++j) {
            ss << (*this)(i, j);
            if (j + 1 < cols()) {
                ss << "\t";
            }
        }
        if (i + 1 < rows()) {
            ss << "\n";
        }
    }
    return ss.str();
}

// Friend operators
template<typename U>
Matrix<U> operator+(const Matrix<U>& A, const Matrix<U>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) + B(i, j);
        }
    }
    return C;
}

template<typename U>
Matrix<U> operator-(const Matrix<U>& A, const Matrix<U>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) - B(i, j);
        }
    }
    return C;
}

template<typename U>
Matrix<std::complex<U>> operator+(const Matrix<U>& A, const Matrix<std::complex<U>>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<std::complex<U>> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) + B(i, j);
        }
    }
    return C;
}

template<typename U>
Matrix<std::complex<U>> operator-(const Matrix<U>& A, const Matrix<std::complex<U>>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<std::complex<U>> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) - B(i, j);
        }
    }
    return C;
}

template<typename U, typename V>
Matrix<U> operator*(const U& c, const Matrix<V>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = c * A(i, j);
    return C;
}

template<typename U>
Matrix<U> operator*(const Matrix<U>& A, const Matrix<U>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) * B(i, j);
        }
    }
    return C;
}

template<typename U, typename V>
Matrix<std::complex<V>> operator*(const Matrix<U>& A, const Matrix<std::complex<V>>& B) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<std::complex<V>> C(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            C(i, j) = A(i, j) * B(i, j);
        }
    }
    return C;
}

template<typename U>
Matrix<U> cos(const Matrix<U>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = std::cos(A(i, j));
    return C;
}

template<typename U>
Matrix<U> sin(const Matrix<U>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = std::sin(A(i, j));
    return C;
}

template<typename U>
Matrix<U> abs(const Matrix<U>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = std::abs(A(i, j));
    return C;
}

template<typename U>
Matrix<U> abs(const Matrix<std::complex<U>>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = std::abs(A(i, j));
    return C;
}

template<typename U>
Matrix<U> angle(const Matrix<std::complex<U>>& A) {
    size_t rows = A.m_Rows;
    size_t cols = A.m_Cols;
    Matrix<U> C(rows, cols);
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            C(i, j) = std::arg(A(i, j));
    return C;
}

#endif
