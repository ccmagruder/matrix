// Copyright 2023 Caleb Magruder

#pragma once

#include <cmath>

#include <iostream>
#include <memory>   // std::shared_ptr
#include <utility>  // std::forward

class EmptyClass{};

#define EMPTY (EmptyClass())

// Defines a collection of matrix operations to be inherited by
// a base class via the Curiously Recurring Template Pattern (CRTP)
template <typename T>
class OperatorSet{
 public:
    OperatorSet() {}

    OperatorSet(ptrdiff_t m, ptrdiff_t n) : _m(m), _n(n) {
        if (static_cast<T*>(this)->__alloc())
            throw(1);
    }

    explicit OperatorSet(ptrdiff_t m) : OperatorSet<T>(m, 1) {}

    ~OperatorSet() {
        static_cast<T*>(this)->__dealloc();
    }

    OperatorSet(EmptyClass e) {}

    explicit OperatorSet<T>(const T& B)
        : OperatorSet<T>(B._m, B._n) {
            if (static_cast<T*>(this)->__copy(B._data, 1))
                throw(1);
        }

    OperatorSet(T&& B)
        : OperatorSet<T>(0, 0) {
            _m = B._m;
            _n = B._n;
            _data = B._data;
            B._m = 0;
            B._n = 0;
            B._data = nullptr;
        }

    // Custom pointer that ignores column/row indexing, enabling
    // double indexing for matrices by pointing to first element in
    // a row or vector access
    // Example:
    //    A[i][j] = value; // A is an (m x n)-matrix
    //    A[i] = value;    // A is an (m x 1)-vector
    class TPtr {
     public:
        // Constructor with address to point to
        explicit TPtr(double* data):_data(data) {}

        // Indexing operator
        double& operator[](ptrdiff_t i) { return _data[i]; }
        const double& operator[](ptrdiff_t i) const { return _data[i]; }

        // Assignment to point location, used to access vectors with
        // with a single index.
        // Example:
        //    A[i] = value;    // A is an (m x 1)-vector
        TPtr& operator=(double a) {
            _data[0] = a;
            return *this;
        }

        // [DELETED] Copy assignment operator from TPtr
        TPtr& operator=(const TPtr& a) = delete;

        // Conversion operator double a = x[0];
        // Throws exception if x is not a column matrix (_n==1)
        operator double&() const {
            return _data[0];
        }

     private:
        double* _data = nullptr;
    };

    // Conversion to access private _data
    operator double*() const {
        return _data;
    }

    // Number of columns
    const ptrdiff_t& cols() const {
        return _n;
    }

    // Number of rows
    const ptrdiff_t& rows() const {
        return _m;
    }

    // Custom pointer to first element in i-th row
    TPtr operator[](ptrdiff_t i) {
        double* ptr = static_cast<double*>(*this) + i*this->cols();
        return TPtr(ptr);
    }
    const TPtr operator[](ptrdiff_t i) const {
        double* ptr = static_cast<double*>(*this) + i*this->cols();
        return TPtr(ptr);
    }

    // Equality Operator: A==B
    friend bool operator==(const T& A, const T& B) {
        // Verify dimensions match
        if (A.rows() != B.rows() || A.cols() != B.cols()) {
            return false;
        }

        // Check that if one is empty, both are
        double* Aptr = static_cast<double*>(A);
        double* Bptr = static_cast<double*>(B);
        if (Aptr == nullptr) {
            return Bptr == nullptr;
        } else if (Bptr == nullptr) {
            return false;
        }

        // Element-wise comparison
        for (ptrdiff_t i = 0; i < numel(A); i++) {
            if (Aptr[i] != Bptr[i]) {
                return false;
            }
        }
        return true;
    }

    // Inequality Operator: A!=B
    bool operator!=(const T& B) const {
        return !(*static_cast<const T*>(this) == B);
    }

    // Multiplication Operator: A*B
    // Allocates Memory for Return Value
    T operator*(const T& B) const {
        if (this->cols() != B.rows()) throw(1);
        T C(this->rows(), B.cols());
        mprod(*static_cast<const T*>(this), B, &C);
        return C;
    }

    // Matrix Product: C = A * B
    // Does Not Allocate, Write In Place
    friend void mprod(const T& A, const T& B, T* C) {
        if (C->rows() != A.rows()) throw(1);
        if (A.cols() != B.rows()) throw(1);
        if (B.cols() != C->cols()) throw(1);
        if (A.__mult(false, false, 1.0, B, C)) throw(1);
    }

    friend void mprod(const T& A, const T& B, T* C, ptrdiff_t ldc) {
        if (C->cols() != ldc) throw(1);
        if (A.cols() != B.rows()) throw(1);
        if (B.cols() > C->cols()) throw(1);
        if (A.__mult(false, false, 1.0, B, C)) throw(1);
    }

    friend void mprod(const bool transA, const bool transB,
            const double alpha, const T& A, const T& B, T* C) {
        // Case 1: No Transposes
        if (!transA && !transB) {
            if (A.rows() != C->rows()) throw (1);
            if (A.cols() != B.rows()) throw(1);
            if (B.cols() != C->cols()) throw(1);
        }
        // Case 2: A Transposed
        else if (transA && !transB) {
            if (A.cols() != C->rows()) throw (1);
            if (A.rows() != B.rows()) throw(1);
            if (B.cols() != C->cols()) throw(1);
        }
        // Case 3: B Transposed
        else if (!transA && transB) {
            if (A.rows() != C->rows()) throw(1);
            if (A.cols() != B.cols()) throw(1);
            if (B.rows() != C->cols()) throw(1);
        }
        // Case 3: A and B Transposed
        else {
            if (A.cols() != C->rows()) throw(1);
            if (A.rows() != B.cols()) throw(1);
            if (B.rows() != C->cols()) throw(1);            
        }
        if (A.__mult(transA, transB, alpha, B, C)) throw(1);
    }

    friend void msub(const T& A, const T& B, T* C) {
        if (A.rows() != B.rows()) throw(1);
        if (A.rows() != C->rows()) throw(1);
        if (A.cols() != B.cols()) throw(1);
        if (A.cols() != C->cols()) throw(1);
        if (A.__sub(B, C)) throw(1);
    }

    // Hadmard Product: C = A .* B
    friend void hprod(const T& A, const T& B, T* C) {
        if (A.rows() != B.rows() || B.rows() != C->rows()) throw(1);
        if (A.cols() != B.cols() || B.cols() != C->cols()) throw(1);
        A.__hprod(B, C);
    }

    // MAXPY: B += alpha * A
    friend void maxpy(const double alpha, const T& A, const ptrdiff_t inca, T* B) {
        if (A.rows() != B->rows()) throw(1);
        if (A.cols() != B->cols()) throw(1);
        if (inca != 1) throw(1);
        B->__daxpy(alpha, A, 1);
    }

    friend void maxpy(const double alpha, double* A, const ptrdiff_t inca, T* B) {
        B->__daxpy(alpha, A, inca);
    }

    // MGER: A += x * y^T
    friend void mger(const double alpha, const T& x, const T& y, T* A) {
        if (numel(x) != A->rows()) throw(1);
        if (numel(y) != A->cols()) throw(1);
        A->__dger(alpha, x, y);
    }

    // MCOPY: B = A
    friend void mcopy(double* A, const ptrdiff_t inca, T* B) {
        if (inca != 0) throw(1);
        B->__copy(A, inca);
    }

    friend void mcopy(const T& A, T* B) {
        if (A.rows() != B->rows()) throw(1);
        if (A.cols() != B->cols()) throw(1);
        B->__copy(A, 1);
    }

    // Dot Product
    friend double dot(const T& A, const T& B) {
        if (A.rows() != B.rows()) throw(1);
        if (A.cols() != B.cols()) throw(1);
        double d;
        A.__dot(B, &d);
        return d;
    }

    // Frobenius Matrix Norm Computation
    friend double norm(const T& A) {
        double n;
        A.__norm(&n);
        return n;
    }

    // Hyperbolic Tangent
    friend void tanh(T* A) {
        A->__tanh();
    }

    // Addition operator: A+=B
    T& operator+=(const T& B) {
        T* A = static_cast<T*>(this);
        if (this->rows() != B.rows() || this->cols() != B.cols())
            throw(1);
        if (A->__daxpy(1.0, B, 1))
            throw(1);
        return *A;
    }

    // Subtraction Operator: A-=B
    T& operator-=(const T& B) {
        T* A = static_cast<T*>(this);
        if (this->rows() != B.rows() || this->cols() != B.cols())
            throw(1);
        if (A->__sub(B, A))
            throw(1);
        return *A;
    }

    // Move Operator: B = std::move(A)
    T& operator=(T&& A) {
        T* B = static_cast<T*>(this);
        if (this != &A) {  // Ignore A = std::move(A);
            B->__dealloc();
            B->_data = A._data;
            B->_m = A._m;
            B->_n = A._n;
            A._data = nullptr;
            A._m = 0;
            A._n = 0;
        }
        return *B;
    }

    // Deep Copy Assignment: A = B
    // Disabled to avoid accidental copy assignment
    // Use copy operator instead (e.g. A = Matrix(B))
    // T& operator=(const T& B) = delete;

    // Number of elements (rows * cols)
    friend const ptrdiff_t numel(const OperatorSet<T>& A) {
        return A.rows() * A.cols();
    }

    // Fill matrix with passed value
    void fill(double value) {
        for (int i = 0; i < numel(*this); i++) {
            static_cast<double*>(*this)[i] = value;
        }
    }

    // Matrix Transpose (Allocates Memory)
    friend T transpose(const T& X) {
        T Y(X.cols(), X.rows());
        for (ptrdiff_t i = 0; i < Y.rows(); i++) {
            for (ptrdiff_t j = 0; j < Y.cols(); j++) {
                Y[i][j] = X[j][i];
            }
        }
        return Y;
    }

    // Serialize: [m,n,data]
    friend std::ostream& operator<<(std::ostream& os, OperatorSet<T>& A) {
        os.write(reinterpret_cast<const char*>(&A.rows()), sizeof(ptrdiff_t));
        os.write(reinterpret_cast<const char*>(&A.cols()), sizeof(ptrdiff_t));
        os.write(reinterpret_cast<const char*>(static_cast<double*>(A)),
                 numel(A)*sizeof(double));
        return os;
    }

    // Deserialize
    friend std::istream& operator>>(std::istream& is, OperatorSet<T>& A) {
        ptrdiff_t rows, cols;
        is.read(reinterpret_cast<char*>(&rows), sizeof(ptrdiff_t));
        is.read(reinterpret_cast<char*>(&cols), sizeof(ptrdiff_t));
        // Allocate memory
        static_cast<T&>(A) = T(rows, cols);
        is.read(reinterpret_cast<char*>(static_cast<double*>(A)),
                rows*cols*sizeof(double));
        return is;
    }

 protected:
    ptrdiff_t _m = 0;
    ptrdiff_t _n = 0;
    double* _data = nullptr;
};

// Scalar Multiply
template <typename T>
T&& operator*(const double alpha, OperatorSet<T>&& A) {
    T&& AA = static_cast<T&&>(A);
    AA.__mult(alpha);
    return std::move(AA);
}
template <typename T>
T&& operator*(const double alpha, OperatorSet<T>& A) {
    return alpha*std::move(A);
}

// Add, std::move(A) + B
template <typename T>
T&& operator+(OperatorSet<T>&& A, const OperatorSet<T>& B) {
    T&& AA = static_cast<T&&>(A);
    const T& BB = static_cast<const T&>(B);
    if (A.rows() != B.rows() || A.cols() != B.cols())
        throw(1);
    AA+=BB;
    return std::move(AA);
}

// Add: A + std::move(B)
template <typename T>
T&& operator+(const OperatorSet<T>& A, OperatorSet<T>&& B) {
    return std::move(static_cast<T&&>(B))+static_cast<const T&>(A);
}

// [DELETED] Add: A + B
template <typename T>
T operator+(OperatorSet<T>& A, OperatorSet<T>& B) = delete;

// Subtract: std::move(A) - B
template <typename T>
T&& operator-(OperatorSet<T>&& A, const OperatorSet<T>& B) {
    T&& AA = static_cast<T&&>(A);
    const T& BB = static_cast<const T&>(B);
    if (A.rows() != B.rows() || A.cols() != B.cols()) throw(1);
    AA -= BB;
    return std::move(AA);
}

// Subtract: A - std::move(B)
template <typename T>
T&& operator-(const OperatorSet<T>& A, OperatorSet<T>&& B) {
    return -1*(std::move(static_cast<T&&>(B))-static_cast<const T&>(A));
}

// [DELETED] Subtract: A - B
template <typename T>
T operator-(const OperatorSet<T>& A, const OperatorSet<T>& B) = delete;
