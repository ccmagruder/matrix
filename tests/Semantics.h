// Copyright 2023 Caleb Magruder

#include <utility>  // std::move

#pragma once

namespace Semantics {

template <typename T>
void empty(const T& e) {
    T A(e);
    EXPECT_EQ(A.rows(), 0);
    EXPECT_EQ(A.cols(), 0);
    EXPECT_EQ(static_cast<double*>(A), nullptr);
}

template <typename T>
void move(const T& Reference) {
    T a(Reference), B(EMPTY);

    // Move to C empties B
    B = std::move(a);
    EXPECT_EQ(a, EMPTY);
    EXPECT_EQ(Reference, B);

    // Self-Move does nothing
    B = std::move(B);
    EXPECT_EQ(Reference, B);
}

template <typename S, typename T = S>
void copy(const T& b) {  // NOLINT [build/include_what_you_use]
    S a(b);

    // Copy instances equal
    EXPECT_EQ(a, b);

    // Copy instance has distinct memory
    static_cast<double*>(a)[0]++;
    EXPECT_FALSE(a == b);
}

template <typename S, typename T = S>
void equality(const T& b) {
    S a(b);

    // Compare equal instances
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    // Compare unequal instances
    static_cast<double*>(a)[0]++;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);

    // Compare vs flattened row vector
    S c(b);
    c = S(numel(b), 1);
    EXPECT_FALSE(b == c);
}

template <typename T>
void scalarMultiply(const T& a) {
    double alpha = 3.14;

    T x(a), y(a);

    // Write in place
    alpha * x;
    const ptrdiff_t n = numel(a);
    for (ptrdiff_t i = 0; i < n; i++)  // NOLINT
        EXPECT_EQ(alpha*static_cast<double*>(a)[i],
                  static_cast<double*>(x)[i]);

    // Move
    T z = alpha * y;
    EXPECT_EQ(z, x);
    // Accessing x after it's been moved gets flagged by clang-tidy's analyzer
    // so we include for testing but exclude for static code analysis
    #ifndef __clang_analyzer__
    EXPECT_EQ(y, EMPTY);
    #endif
}

template <typename T>
void additionEquals(const T& a) {
    T x(a), y(a);

    // Compare equivalent
    x += y;
    2*y;  // Write-in-place
    EXPECT_EQ(x, y);

    // Verify distinct allocation
    static_cast<double*>(x)[0]++;
    EXPECT_NE(x, y);
    EXPECT_THROW(y+=EMPTY, int);  // Wrong dims

    // Flatten shape mismatch
    x = T(numel(y), 1);
    EXPECT_THROW(y+=x, int);  // Wrong dims
}

template <typename T>
void maxpy(const T& a) {
    T b(a);
    maxpy(-2.0, a, 1, &b);  // b -= 2*a
    EXPECT_EQ(-1*T(a), b);

    T c(a.rows(), a.cols()), d(a.rows(), a.cols());
    c.fill(0);
    d.fill(1);
    double two(2.0);
    maxpy(0.5, &two, 0, &c);
}

template <typename T>
void mger(const T& a) {
    T x(2); x[0] = 1; x[1] = 2;
    T y(2); y[0] = 2; y[1] = 3;
    T b(a), c(a);
    mger(1.0, x, y, &b);
    c[0][0] += 2;
    c[0][1] += 3;
    c[1][0] += 4;
    c[1][1] += 6;
    EXPECT_EQ(b, c);
}

template <typename T>
void mcopy(const T& a) {
    T b(a.rows(), a.cols());
    b.fill(0);
    mcopy(a, &b);
    EXPECT_EQ(a, b);

    double pi(3.14);
    T c(a.rows(), a.cols());
    T d(a.rows(), a.cols());
    c.fill(0);
    d.fill(pi);
    mcopy(&pi, 0, &c);
    EXPECT_EQ(c, d);
}

template <typename T>
void subtraction(const T& a, const T& b, const T& c) {
    T d(a.rows(), a.cols());
    d.fill(0);
    msub(a, b, &d); // c = b - a (in place)
    EXPECT_EQ(c, d);
}

template <typename T>
void subtractionEquals(const T& a) {
    T x(a), y(a), zeros(a);
    zeros.fill(0);

    // Compare equivalent
    x -= y;

    EXPECT_EQ(x, zeros);

    // Verify distinct allocation
    static_cast<double*>(x)[0]++;
    EXPECT_NE(x, y);
    EXPECT_THROW(y -= EMPTY, int);  // Wrong dims

    // Flatten shape mismatch
    x = T(numel(y), 1);
    EXPECT_THROW(y -= x, int);  // Wrong dims
}

template <typename T>
void addition(const T& a) {
    T x(a), y(a);
    T z = std::move(x) + y;
    2*y;  // Write-in-place
    EXPECT_EQ(z, y);
    // Accessing x after it's been moved gets flagged by clang-tidy's analyzer
    // so we include for testing but exclude for static code analysis
    #ifndef __clang_analyzer__
    EXPECT_EQ(x, EMPTY);
    #endif

    x = T(a);
    y = T(a);
    z = x + std::move(y);
    2*x;  // Write-in-place
    EXPECT_EQ(z, x);
    // Accessing x after it's been moved gets flagged by clang-tidy's analyzer
    // so we include for testing but exclude for static code analysis
    #ifndef __clang_analyzer__
    EXPECT_EQ(y, EMPTY);
    #endif

    x = T(a);
    y = T(a);
    z = x + T(y);
    EXPECT_EQ(x, a);
    EXPECT_EQ(y, a);
    2*x;
    EXPECT_EQ(z, x);

    x = T(a);
    y = T(a);
    z = T(x) + y;
    EXPECT_EQ(x, a);
    EXPECT_EQ(y, a);
    2*y;
    EXPECT_EQ(z, y);

    x = T(a);
    z = T(numel(x), 1);

    EXPECT_THROW(x+std::move(z), int);  // Wrong dims
    EXPECT_THROW(std::move(z)+x, int);  // Wrong dims
}

template <typename T>
void subtraction(const T& a) {
    T x(a), y(a);
    y = 2*y;
    T z = std::move(x) - y;
    EXPECT_EQ(z, -1*T(a));
    // Accessing x after it's been moved gets flagged by clang-tidy's analyzer
    // so we include for testing but exclude for static code analysis
    #ifndef __clang_analyzer__
    EXPECT_EQ(x, EMPTY);
    #endif

    x = T(a);
    y = 2*T(a);
    z = x - std::move(y);
    EXPECT_EQ(z, -1*T(a));
    // Accessing x after it's been moved gets flagged by clang-tidy's analyzer
    // so we include for testing but exclude for static code analysis
    #ifndef __clang_analyzer__
    EXPECT_EQ(y, EMPTY);
    #endif

    x = T(a);
    y = 2*T(a);
    z = x - T(y);
    EXPECT_EQ(x, a);
    EXPECT_EQ(y, 2*T(a));
    EXPECT_EQ(z, -1*x);

    x = T(a);
    y = 2*T(a);
    z = T(x) - y;
    EXPECT_EQ(x, a);
    EXPECT_EQ(y, 2*T(a));
    EXPECT_EQ(z, -1*x);

    x = T(a);
    z = T(numel(a), 1);

    EXPECT_THROW(x-std::move(z), int);  // Wrong dims
    EXPECT_THROW(std::move(z)-x, int);  // Wrong dims
}

template <typename S, typename T = S>
void multiplication(const S& a, const T& b, const T& c) {
    T d = a*b;
    EXPECT_EQ(d, c);
    T e(a.rows(), b.cols());
    mprod(a, b, &e);
    EXPECT_EQ(e, c);
}

template <typename S, typename T = S>
void multiplication(const bool transa, const bool transb, const double alpha,
        const S& a, const T& b, const T& c) {
    T d(c.rows(), c.cols());
    mprod(transa, transb, alpha, a, b, &d);
    EXPECT_EQ(d, c);
}

template <typename S, typename T = S>
void multiplicationLeftColsOnly(const S& a, const T& b, const T& c) {
    T e(c);
    // Zero out leading [b.cols()] columns of e
    // E[:,1:b.cols()] = 0
    for (ptrdiff_t i = 0; i < e.rows(); i++) {
        for (ptrdiff_t j = 0; j < b.cols(); j++) {
            e[i][j] = 0;
        }
    }
    // E[:,1:b.cols()] = A * B
    mprod(a, b, &e, c.cols());
    // Check E = [ A*B, C[:, b.cols():1] ]
    for (ptrdiff_t i = 0; i < e.rows(); i++) {
        for (ptrdiff_t j = 0; j < e.cols(); j++) {
            EXPECT_EQ(e[i][j], c[i][j]);
        }
    }
}

template <typename T>
void hadamardMultiplication(const T& a, const T&b, const T& c) {
    // Write in place
    T d(a);
    hprod(d, b, &d);
    for (ptrdiff_t i = 0; i < numel(b); i++)
        EXPECT_EQ(static_cast<double*>(d)[i],
                  static_cast<double*>(c)[i]);
}

template <typename T>
void fill() {
    T x(100, 20);
    double alpha = 3.14;
    x.fill(alpha);
    for (ptrdiff_t i = 0; i < x._m*x._n; i++) {
        EXPECT_EQ(static_cast<double*>(x)[i], alpha);
    }
}

}  // namespace Semantics
