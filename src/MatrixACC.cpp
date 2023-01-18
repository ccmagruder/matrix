// Copyright 2022 Caleb Magruder

#include <Accelerate/Accelerate.h>

#include <cassert>

#include "Matrix.h"

template<> int Matrix<ACC>::__alloc() {
    if (_m*_n > 0) {
        _data = new double[_m*_n];
    }
    return 0;  // Successful Allocation
}

template<> int Matrix<ACC>::__copy(double* A, const ptrdiff_t inca) {
    // _data = copy(A._data)
    cblas_dcopy(_m * _n,  // n
                A,        // x
                inca,     // incx
                _data,    // y
                1);    // incy
    return 0;  // Successful Copy
}

template<> int Matrix<ACC>::__daxpy(const double alpha,
                                    const double* B,
                                    const ptrdiff_t incb) {
    cblas_daxpy(_m * _n,  // N
                alpha,    // alpha
                B,        // X
                incb,     // incX
                _data,    // Y);
                1);       // incY
    return 0;
}

template<> int Matrix<ACC>::__dger(const double alpha,
                                   const Matrix<ACC>& x,
                                   const Matrix<ACC>& y) {
    cblas_dger(CblasRowMajor,  // Layout
               this->_m,       // m
               this->_n,       // n
               alpha,          // alpha
               x._data,        // x
               1,              // incx
               y._data,        // y
               1,              // incy
               _data,          // a
               this->_n);       // lda
    return 0;
}

template<> int Matrix<ACC>::__dealloc() {
    if (_data != nullptr) {
        // mkl_free(_data);
        delete _data;
    }
    return 0;  // Successful Deallocation
}

template<> int Matrix<ACC>::__dot(const Matrix<ACC>& B, double* d) const {
    *d = cblas_ddot(this->rows() * this->cols(), this->_data, 1, B._data, 1);
    return 0;
}

template<> int Matrix<ACC>::__hprod(const Matrix<ACC>& B,
                                    Matrix<ACC>* C) const {
    vDSP_vmulD(*this, 1,
               B, 1,
               *C, 1,
               this->rows() * this->cols());
    return 0;
}

template<> int Matrix<ACC>::__mult(const bool transA,
        const bool transB,
        const double alpha,
        const Matrix<ACC>& B,
        Matrix<ACC>* C) const {
    cblas_dgemm(CblasRowMajor,                   // Layout
            transA ? CblasTrans : CblasNoTrans,  // transa
            transB ? CblasTrans : CblasNoTrans,  // transb
            C->_m,                               // m
            transB ? B._m : B._n,                // n
            transB ? B._n : B._m,                // k
            alpha,                               // alpha
            _data,                               // a
            _n,                                  // lda
            B._data,                             // b
            B._n,                                // ldb
            0,                                   // beta
            C->_data,                            // c
            C->_n);                              // ldc
    return 0;
}

template<> int Matrix<ACC>::__mult(const double alpha) {
    const int n(_m*_n), incx(1);
    cblas_dscal(n,      // n
                alpha,  // alpha
                _data,  // data
                incx);  // incx
    return 0;  // Successful Multiply
}

template<> int Matrix<ACC>::__norm(double* n) const {
    *n = cblas_dnrm2(this->rows() * this->cols(), this->_data, 1);
    return 0;
}

template<> int Matrix<ACC>::__sub(const Matrix<ACC>& B, Matrix<ACC>* C) const {
    // C = A - B
    vDSP_vsubD(B._data,     // __B
               1,           // __IB
               _data,       // __A
               1,           // __IA
               C->_data,    // __C
               1,           // __IC
               numel(*C));  // __N
    return 0;  // Successful Subtraction
}

template<> int Matrix<ACC>::__tanh() {
    const int n = this->rows() * this->cols();
    vvtanh(*this, *this, &n);
    return 0;
}
