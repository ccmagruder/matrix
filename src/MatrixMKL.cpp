// Copyright 2022 Caleb Magruder

#include <mkl.h>

#include <cassert>

#include "Matrix.h"

template<> int Matrix<MKL>::__alloc() {
    if (_m*_n > 0) {
        void* ptr = mkl_malloc( _m*_n*sizeof( double ), 64);
        _data = reinterpret_cast<double *>(ptr);
    }
    return 0;  // Successful Allocation
}

template<> int Matrix<MKL>::__copy(double* A, const ptrdiff_t inca) {
    // _data = copy(B._data)
    const MKL_INT incx(inca), incy(1);
    cblas_dcopy(_m * _n,  // n
                A,        // x
                incx,     // incx
                _data,    // y
                incy);    // incy
    return 0;  // Successful Copy
}

template<> int Matrix<MKL>::__daxpy(const double alpha,
                                    const double* B,
                                    const ptrdiff_t incb) {
    cblas_daxpy(_m * _n,  // N
                alpha,    // alpha
                B,  // X
                incb,     // incX
                _data,    // Y);
                1);       // incY
    return 0;
}

template<> int Matrix<MKL>::__dger(const double alpha,
                                   const Matrix<MKL>& x,
                                   const Matrix<MKL>& y) {
    cblas_dger(CblasRowMajor,  // Layout
               this->_m,       // m
               this->_n,       // n
               alpha,          // alpha
               x._data,        // x
               1,              // incx
               y._data,        // y
               1,              // incy
               _data,          // a
               this->_n);      // lda
    return 0;
}

template<> int Matrix<MKL>::__dealloc() {
    if (_data != nullptr) {
        mkl_free(_data);
    }
    return 0;  // Successful Deallocation
}

template<> int Matrix<MKL>::__dot(const Matrix<MKL>& B, double* d) const {
    *d = cblas_ddot(this->rows() * this->cols(), this->_data, 1, B._data, 1);
    return 0;
}

template<> int Matrix<MKL>::__hprod(const Matrix<MKL>& B,
                                    Matrix<MKL>* C) const {
    vdMul(this->rows() * this->cols(), *this, B, *C);
    return 0;
}

template<> int Matrix<MKL>::__mult(const bool transA,
        const bool transB,
        const double alpha,
        const Matrix<MKL>& B,
        Matrix<MKL>* C) const {
    cblas_dgemm(CblasRowMajor,                       // Layout
                transA ? CblasTrans : CblasNoTrans,  // transa
                transB ? CblasTrans : CblasNoTrans,  // transb
                _m,                                  // m
                B._n,                                // n
                _n,                                  // k
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

template<> int Matrix<MKL>::__mult(const double alpha) {
    const MKL_INT n(_m*_n), incx(1);
    cblas_dscal(n,      // n
                alpha,  // alpha
                _data,  // data
                incx);  // incx
    return 0;  // Successful Multiply
}

template<> int Matrix<MKL>::__norm(double* n) const {
    *n = cblas_dnrm2(this->rows() * this->cols(), this->_data, 1);
    return 0;
}

template<> int Matrix<MKL>::__sub(const Matrix<MKL>& B, Matrix<MKL>* C) const {
    // A -= B
    vdSub(_m*_n,      // n
          _data,      // a
          B._data,    // b
          C->_data);  // y
    return 0;  // Successful Subtraction
}

template<> int Matrix<MKL>::__tanh() {
    vmdTanh(this->rows() * this->cols(), *this, *this, VML_HA);
    return 0;
}
