// Copyright 2023 Caleb Magruder

#include <cstdio>
#include <list>
#include <fstream>

#include "gtest/gtest.h"

#include "Matrix.h"
#include "Semantics.h"
#include "TestWithLogging.h"

/////////////////////////////////////////
// Helper Functions
/////////////////////////////////////////
template<typename T>
T build2x2() {
    T A(2, 2);
    A[0][0] = 0;
    A[0][1] = 1;
    A[1][0] = -1;
    A[1][1] = 0;
    return A;
}

template<typename T>
T build2x2Squared() {
    T A(2, 2);
    A[0][0] = -1;
    A[0][1] = 0;
    A[1][0] = 0;
    A[1][1] = -1;
    return A;
}

/////////////////////////////////////////
// tMatrix Fixture
/////////////////////////////////////////
template <typename T>
class tMatrix : public TestWithLogging {};

/////////////////////////////////////////
// tMatrixPtr Fixture
/////////////////////////////////////////
template <typename T>
class tMatrixPtr : public TestWithLogging {};

    using MyTypes = ::testing::Types
            < Matrix<REF>
        #if ACC_FOUND
                ,Matrix<ACC>
        #endif
        #if OPB_FOUND
                ,Matrix<OPB>
        #endif
        #if MKL_FOUND
                ,Matrix<MKL>
        #endif
            >;

TYPED_TEST_SUITE(tMatrix, MyTypes);
TYPED_TEST_SUITE(tMatrixPtr, MyTypes);

/////////////////////////////////////////
// C = A * B
/////////////////////////////////////////
TYPED_TEST(tMatrix, MatrixMultiplicationOperator) {
    // C = A * B
    TypeParam A = build2x2<TypeParam>();
    TypeParam B = build2x2Squared<TypeParam>();
    Semantics::multiplication<TypeParam>(A, A, B);

    // y = A * x
    TypeParam x(2), y(2);
    x[0] = 2; x[1] = 1;
    y[0] = 1; y[1] = -2;
    Semantics::multiplication<TypeParam>(A, x, y);

    // Y = A * x [Write Left-Most Columns of Y]
    TypeParam Y(2, 2);
    Y[0][0] =  1;  Y[0][1] = -3;
    Y[1][0] = -2;  Y[1][1] =  5;
    Semantics::multiplicationLeftColsOnly<TypeParam>(A, x, Y);

    // I = C^T * C
    // I = C * C^T
    TypeParam C(3, 3);  // Circulant Shift Operator
    C.fill(0);
    C[1][0] = 1; C[2][1] = 1; C[0][2] = 1;
    TypeParam I(3, 3);  // Identity Matrix
    I.fill(0);
    I[0][0] = 1; I[1][1] = 1; I[2][2] = 1;
    Semantics::multiplication<TypeParam>(true, false, 1.0, C, C, I);
    Semantics::multiplication<TypeParam>(false, true, 1.0, C, C, I);

    // 2.0 * C * C^T = 2.0 * I
    TypeParam D(I);
    Semantics::multiplication<TypeParam>(false, true, 2.0, C, C, 2*D);
}

/////////////////////////////////////////
// hprod(A, B, &C)
/////////////////////////////////////////
TYPED_TEST(tMatrix, HadamardMultiplicationOperator) {
    TypeParam A = build2x2<TypeParam>();
    TypeParam B = TypeParam(2, 2);
    for (ptrdiff_t i = 0; i < numel(B); i++) {
        static_cast<double*>(B)[i]
            = static_cast<double*>(A)[i] * static_cast<double*>(A)[i];
    }
    Semantics::hadamardMultiplication<TypeParam>(A, A, B);
}

/////////////////////////////////////////
// A == B
// A != B
/////////////////////////////////////////
TYPED_TEST(tMatrix, EqualityOperator) {
    Semantics::equality<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// TypeParam A(TypeParam B)
/////////////////////////////////////////
TYPED_TEST(tMatrix, CopyConstructor) {
    Semantics::copy<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// B += A
/////////////////////////////////////////
TYPED_TEST(tMatrix, MatrixPlusEqualsOperator) {
    Semantics::additionEquals<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// daxpy(alpha, A, inca, &B)    [B += A]
/////////////////////////////////////////
TYPED_TEST(tMatrix, MaxpyOperator) {
    Semantics::maxpy<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// dger(alpha, x, y, &A)
/////////////////////////////////////////
TYPED_TEST(tMatrix, MgerOperator) {
    Semantics::mger<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// mcopy(A, inca, &B)
/////////////////////////////////////////
TYPED_TEST(tMatrix, McopyOperator) {
    Semantics::mcopy<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// msub(A, B, &C) // C = B - A (no copy/move)
// A -= B
/////////////////////////////////////////
TYPED_TEST(tMatrix, MatrixMinusEqualsOperator) {
    TypeParam A = build2x2<TypeParam>();
    TypeParam Z(2, 2);
    Z.fill(0);
    Semantics::subtraction<TypeParam>(A, A, Z);
    Semantics::subtractionEquals<TypeParam>(A);
}

/////////////////////////////////////////
// C = std::move(A) + B
// C = A + std::move(B)
// C = TypeParam(A) + B;
/////////////////////////////////////////
TYPED_TEST(tMatrix, AdditionOperator) {
    Semantics::addition<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// C = std::move(A) + B
// C = A + std::move(B)
// C = TypeParam(A) + B;
/////////////////////////////////////////
TYPED_TEST(tMatrix, SubtractionOperator) {
    Semantics::subtraction<TypeParam>(build2x2<TypeParam>());
}
/////////////////////////////////////////
// A = std::move(B)
/////////////////////////////////////////
TYPED_TEST(tMatrix, MoveAssignment) {
    Semantics::move<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// TypeParam A(EMPTY)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Empty) {
    Semantics::empty<TypeParam>(EMPTY);
}

/////////////////////////////////////////
// A.fill(double)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Fill) {
    ptrdiff_t m = 20, n = 50;
    TypeParam A(m, n);
    A.fill(3);
    for (ptrdiff_t i = 0; i < m; i++)
        for (ptrdiff_t j = 0; j < n; j++)
            EXPECT_EQ(A[i][j], 3);
}

/////////////////////////////////////////
// A = std::move(alpha * B)
// alpha * B
/////////////////////////////////////////
TYPED_TEST(tMatrix, ScalarMultiply) {
    Semantics::scalarMultiply<TypeParam>(build2x2<TypeParam>());
}

/////////////////////////////////////////
// x = TypeParam(n,1);
// x[i] = a;
// b = x[i];
/////////////////////////////////////////
TYPED_TEST(tMatrix, ColumnMatrixVectorIndex) {
    TypeParam x(2);
    double a = 4;
    x[0] = a;
    EXPECT_EQ(x[0][0], a);
    double b = x[0];
    EXPECT_EQ(b, a);
    EXPECT_EQ(x[0], a);
    TypeParam y(2, 2);
}

/////////////////////////////////////////
// numel(A)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Numel) {
    ptrdiff_t m(30), n(5);
    TypeParam x(m, n);
    EXPECT_EQ(m*n, numel(x));
}

/////////////////////////////////////////
// tanh(A)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Tanh) {
    TypeParam A = build2x2<TypeParam>();
    std::cerr << "1 A[0][1] = " << A[0][1] << std::endl;
    // A[0][1] --;
    TypeParam B(A);
    std::cerr << "2 A[0][1] = " << A[0][1] << std::endl;
    tanh(&A);
    std::cerr << "5 A[0][1] = " << A[0][1] << std::endl;
    EXPECT_EQ(A[0][0], std::tanh(B[0][0]));
    EXPECT_EQ(A[0][1], std::tanh(B[0][1]));
    EXPECT_EQ(A[1][0], std::tanh(B[1][0]));
    EXPECT_EQ(A[1][1], std::tanh(B[1][1]));
}

/////////////////////////////////////////
// dot(A,B)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Dot) {
    TypeParam x(2), y(2);
    x[0] = 1; x[1] = 1;
    y[0] = 1; y[1] = -1;
    EXPECT_EQ(dot(x, y), 0);
    EXPECT_EQ(dot(x, x), 2);
}

/////////////////////////////////////////
// norm(X)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Norm) {
    TypeParam x(2);
    x[0] = -3; x[1] = 4;
    EXPECT_EQ(norm(x), 5);
}

/////////////////////////////////////////
// transpose(X)
/////////////////////////////////////////
TYPED_TEST(tMatrix, Transpose) {
    ptrdiff_t m = 10, n = 5;
    TypeParam X(m, n);
    for (ptrdiff_t i = 0; i < m*n; i++) {
        static_cast<double*>(X)[i] = i;
    }
    TypeParam Y = transpose(X);
    for (ptrdiff_t i = 0; i < m; i++) {
        for (ptrdiff_t j = 0; j < n; j++) {
            ASSERT_EQ(X[i][j], Y[j][i]);
        }
    }
}

/////////////////////////////////////////
// std::ostream << X
/////////////////////////////////////////
TYPED_TEST(tMatrix, Serialize) {
    TypeParam A = build2x2<TypeParam>();
    const char* fileName("TestSerialize.bin");

    // Delete test binary
    std::remove(fileName);

    // Write A to disk
    std::ofstream ofile(fileName);
    ofile << A;
    ofile.close();

    // Read from disk to A
    std::ifstream ifile(fileName);
    TypeParam B;
    ifile >> B;
    ifile.close();

    ASSERT_EQ(A, B);

    // Delete test binary
    std::remove(fileName);
}

/////////////////////////////////////////
// Ptr<Matrix<T>> ptr(A, m, n);
/////////////////////////////////////////
TYPED_TEST(tMatrixPtr, Constructor) {
    TypeParam* A = new TypeParam(build2x2<TypeParam>());
    typename TypeParam::Ptr ptr1(static_cast<double*>(*A), A->rows(), A->cols());
    typename TypeParam::Ptr* ptr2 = new typename TypeParam::Ptr(static_cast<double*>(*A), A->rows(), A->cols());
    delete ptr2;
    delete A;
}
