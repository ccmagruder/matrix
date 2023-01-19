// Copyright 2023 Caleb Magruder

#include <cblas.h>

#include "Matrix.h"

template<> int Matrix<OPB>::__alloc() {
    if (_m*_n > 0) {
        _data = new double[_m*_n];
    }
    return 0;  // Successful Allocation
}

template<> int Matrix<OPB>::__copy(double* A, const ptrdiff_t inca) {
    // _data = copy(A._data)
    cblas_dcopy(_m * _n,  // n
                A,        // x
                inca,     // incx
                _data,    // y
                1);    // incy
    return 0;  // Successful Copy
}

template<> int Matrix<OPB>::__daxpy(const double alpha,
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

template<> int Matrix<OPB>::__dger(const double alpha,
                                   const Matrix<OPB>& x,
                                   const Matrix<OPB>& y) {
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

template<> int Matrix<OPB>::__dealloc() {
    if (_data != nullptr) {
        // mkl_free(_data);
        delete _data;
    }
    return 0;  // Successful Deallocation
}

template<> int Matrix<OPB>::__dot(const Matrix<OPB>& B, double* d) const {
    *d = cblas_ddot(this->rows() * this->cols(), this->_data, 1, B._data, 1);
    return 0;
}

// template<> int Matrix<OPB>::__hprod(const Matrix<OPB>& B,
//                                     Matrix<OPB>* C) const {
//     return 0;
// }

template<> int Matrix<OPB>::__mult(const bool transA,
        const bool transB,
        const double alpha,
        const Matrix<OPB>& B,
        Matrix<OPB>* C) const {
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

template<> int Matrix<OPB>::__mult(const double alpha) {
    const int n(_m*_n), incx(1);
    cblas_dscal(n,      // n
                alpha,  // alpha
                _data,  // data
                incx);  // incx
    return 0;  // Successful Multiply
}

template<> int Matrix<OPB>::__norm(double* n) const {
    *n = cblas_dnrm2(this->rows() * this->cols(), this->_data, 1);
    return 0;
}

// template<> int Matrix<OPB>::__sub(const Matrix<OPB>& B,
                                  // Matrix<OPB>* C) const {
    // C = A - B
    // return 0;  // Successful Subtraction
// }

// template<> int Matrix<OPB>::__tanh() {
    // Hyperbolic Tangent: *this = tanh(*this)
    // return 0;
// }
