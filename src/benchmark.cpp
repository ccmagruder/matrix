// Copyright 2023 Caleb Magruder

/*
Benchmark Matrix Multiply

The benchmark measures wall clock time of multiplying two square matrices.
    ./benchmark -n100
*/

#include <chrono>
#include <iostream>
#include <string>

#include "Matrix.h"

#include "benchmark/benchmark.h"

template <BLAS T>
void matrixSquared(benchmark::State& state) {
    const int N = state.range(0);
    Matrix<T> A(N, N);
    for (auto _ : state) {
        Matrix<T> B = A * A;
    }
}

BENCHMARK_TEMPLATE(matrixSquared, REF)->Range(4, 256);

#if ACC_FOUND
BENCHMARK_TEMPLATE(matrixSquared, ACC)->Range(4, 256);
#endif

#if OPB_FOUND
BENCHMARK_TEMPLATE(matrixSquared, OPB)->Range(4, 256);
#endif

#if MKL_FOUND == 1
BENCHMARK_TEMPLATE(matrixSquared, MKL)->Range(4, 256);
#endif

BENCHMARK_MAIN();
