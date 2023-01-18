// Copyright 2023 Caleb Magruder

#pragma once

#include <memory>
#include <random>
#include <utility>

#include "OperatorSet.h"

// BLAS Libraries
enum BLAS { REF, ACC, OPB, MKL };

// Maps BLAS::MKL to "MKL"
std::ostream& operator<<(std::ostream& os, BLAS type);

// Matrix Library
template <BLAS T>
class Matrix : public OperatorSet<Matrix<T>> {
 public:
    // Access Parent Constructors
    using OperatorSet<Matrix<T>>::OperatorSet;

    // Deep Copy Constructor: MatrixMKL A(B);
    explicit Matrix<T>(const Matrix<T>& B)
            : OperatorSet<Matrix<T>>(B) {}

    // Move Constructor: MatrixMKL(std::move(B));
    // Needed for Return Value Optimization
    Matrix(Matrix<T>&& B)
        : OperatorSet<Matrix<T>>(std::move(B)) {}

    using OperatorSet<Matrix<T>>::operator=;

    // Deep Copy Assignment: A = B
    // Disabled to avoid accidental copy assignment
    // Use copy operator instead (e.g. A = Matrix(B))
    Matrix<T>& operator=(const Matrix<T>& B) = delete;

    // Random matrix generator
    static Matrix<T> randn(ptrdiff_t m, ptrdiff_t n = 1) {
        Matrix<T> A(m, n);
        for (ptrdiff_t i = 0; i < numel(A); i++) {
            static_cast<double*>(A)[i] = Matrix<T>::randn();
        }
        return A;
    }

    // Allocate Memory
    int __alloc();

    // Deep Copy: *this = A
    int __copy(double* A, const ptrdiff_t inca);

    // DAXPY: A = A + alpha * B
    int __daxpy(const double alpha, const double* B, const ptrdiff_t incb);

    // DGER: A += x * y^T
    int __dger(const double alpha, const Matrix<T>& x, const Matrix<T>& y);

    // Deallocate Memory
    int __dealloc();

    // Doc Product
    int __dot(const Matrix<T>& B, double* d) const;

    // Hadamard Product
    int __hprod(const Matrix<T>& B, Matrix<T>* C) const;

    // Matrix-Matrix Multiply: C = *this * B
    int __mult(const bool transA, const bool transB, const double alpha,
               const Matrix<T>& B, Matrix<T>* C) const;

    // Scalar-Matrix Multiply: *this = alpha * (*this)
    int __mult(const double alpha);

    // Frobenius Matrix Norm
    int __norm(double* n) const;

    // Subtraction: *this -= B
    int __sub(const Matrix<T>& B, Matrix<T>* C) const;

    // Hyperbolic Tangent tanh(&A)
    int __tanh();

    static double randn() {
        static std::random_device rd {};
        static std::mt19937 gen {rd()};
        static std::normal_distribution<> d{0, 1};
        return d(gen);
    }

    class Ptr;
};

// Matrix Pointer -> Ctor / Dtor Does Not Allocate / Deallocate 
template <BLAS T>
class Matrix<T>::Ptr : public Matrix<T> {
public:
    Ptr(double* data, ptrdiff_t m, ptrdiff_t n) {
        // Skip Matrix() ctor to skip allocation
        this->_data = data;
        this->_m = m;
        this->_n = n;
    }

    ~Ptr() {
        // Empty object so that ~Matrix() doesn't deallocate
        this->_data = NULL;
        this->_m = 0;
        this->_n = 0;
    }
};


template<BLAS T> int Matrix<T>::__alloc() {
    ptrdiff_t n = this->rows() * this->cols();
    if (n > 0) {
        this->_data = new double[n];
    }
    return 0;  // Successful Allocation
}

template<BLAS T> int Matrix<T>::__copy(double* A, const ptrdiff_t inca) {
    ptrdiff_t n = this->rows() * this->cols();
    double* ptr = A;
    for (ptrdiff_t i = 0; i < n; i++) {
        this->_data[i] = *ptr;
        ptr += inca;
    }
    // std::memcpy(this->_data, A, n*sizeof(double));
    return 0;  // Successful Copy
}

template<BLAS T> int Matrix<T>::__daxpy(const double alpha, const double* B, const ptrdiff_t incb) {
    for (ptrdiff_t i = 0; i < this->_m * this->_n; i++) {
        this->_data[i] += alpha * B[i];
    }
    return 0;
}

template<BLAS T> int Matrix<T>::__dger(const double alpha, const Matrix<T>& x, const Matrix<T>& y) {
    for (ptrdiff_t i = 0; i < this->_m; i++) {
        for (ptrdiff_t j = 0; j< this->_n; j++) {
            this->operator[](i)[j] += x._data[i] * y._data[j];
        }
    }
    return 0;
}

template<BLAS T> int Matrix<T>::__dealloc() {
    if (this->_data != nullptr) {
        delete this->_data;
    }
    return 0;  // Successful Deallocation
}

template<BLAS T> int Matrix<T>::__dot(const Matrix<T>& B, double* d) const {
    *d = 0;
    for (ptrdiff_t i = 0; i < this->rows() * this->cols(); i++) {
        *d += this->_data[i] * B._data[i];
    }
    return 0;
}

template<BLAS T> int Matrix<T>::__hprod(const Matrix<T>& B, Matrix<T>* C) const {
    for (ptrdiff_t i=0; i< this->rows() * this->cols(); i++) {
        C->_data[i] = this->_data[i] * B._data[i];
    }
    return 0;
}

template<BLAS T> int Matrix<T>::__mult(const bool transA,
        const bool transB,
        const double alpha, 
        const Matrix<T>& B,
        Matrix<T>* C) const {
    double a, b;
    for (ptrdiff_t i = 0; i < this->_m; i++) {
        for (ptrdiff_t j = 0; j < B._n; j++) {
            (*C)[i][j] = 0;
            for (ptrdiff_t k = 0; k < this->_n; k++) {
                a = transA ? this->operator[](k)[i] : this->operator[](i)[k];
                b = transB ? B[j][k] : B[k][j];
                (*C)[i][j] += a * b;
            }
            (*C)[i][j] *= alpha;
        }
    }
    return 0;  // Successful Multiply    
}

template<BLAS T> int Matrix<T>::__mult(const double alpha) {
    for (ptrdiff_t i = 0; i < this->_m*this->_n; i++) {
        this->_data[i] *= alpha;
    }
    return 0;  // Successful Multiply
}

template<BLAS T> int Matrix<T>::__norm(double* n) const {
    this->__dot(*this, n);
    *n = std::sqrt(*n);
    return 0;
}

template<BLAS T> int Matrix<T>::__sub(const Matrix<T>& B, Matrix<T>* C) const {
    for (ptrdiff_t i = 0; i < this->_m*this->_n; i++) {
        C->_data[i] = this->_data[i] - B._data[i];
    }
    return 0;  // Successful Subtraction
}

template<BLAS T> int Matrix<T>::__tanh() {
    for (ptrdiff_t i = 0; i < this->rows() * this->cols(); i++) {
        this->_data[i] = std::tanh(this->_data[i]);
    }
    return 0; 
}