_Copyright 2023 Caleb Magruder_

# Hardware-Accelerated Matrix Algebra

This library is a matrix algebra library with multiple backends for hardware acceleration via each backend's BLAS API.
1. Apple's Accelerate Framework
1. Intel's Math Kernel Library
1. OpenBLAS

A reference implementation is also provided for unit testing, benchmarking, in the event that harware acceleration is unavailable. LAPACK subroutines are not supported.

# Installing

To compile with the reference implementation only the cmake commands are standard.

```
git clone https://www.github.com/ccmagruder/Matrix.git
cd Matrix && mkdir build && cd build
cmake ..
cmake --build .
ctest
```

On MacOS with an Intel architecture it is possible to configure cmake with all four backends simultaneously.
```
cmake ..
```

To benchmark the backends after enabling, call the benchmark utility to measure wall clock time to square a matrix.
```
% ./benchmark
Running build/benchmark
Run on (10 X 24 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 2.45, 2.28, 2.11
-----------------------------------------------------------------
Benchmark                       Time             CPU   Iterations
-----------------------------------------------------------------
matrixSquared<REF>/4         1727 ns         1727 ns       405630
matrixSquared<REF>/8        11895 ns        11893 ns        58869
matrixSquared<REF>/64     5434904 ns      5434609 ns          128
matrixSquared<REF>/256  346896791 ns    346884000 ns            2
matrixSquared<ACC>/4         79.3 ns         79.3 ns      8752188
matrixSquared<ACC>/8          156 ns          156 ns      4279723
matrixSquared<ACC>/64        2396 ns         2395 ns       297782
matrixSquared<ACC>/256     101849 ns       101848 ns         6877
matrixSquared<OPB>/4         79.5 ns         79.5 ns      8780402
matrixSquared<OPB>/8          156 ns          156 ns      4572115
matrixSquared<OPB>/64        2350 ns         2350 ns       299941
matrixSquared<OPB>/256     101762 ns       101762 ns         6868
```
These benchmark results were generated on an Apple M1 Max 64GB Studio.

# Syntax

The Matrix library uses copy and move semantics to prevent unintentional copies and unneccesary mallocs.
Specifically, the equality operator `A=B;` can be interpretted at least three distinct ways:
1. If the dimensions of `A` and `B` match, `B` copies into `A`, overwriting `A`.
1. If the dimensions of `A` and `B` _do not_ match, `A` deallocates, `A` reallocates, `B` copies into `A`. 
1. `A` deallocates, `B` moves into `A`, `B` is left empty.

Therefore the equality operator is disabled due to its ambiguity and we develop three distinct syntaxes each of the interpretations above.
```
A = B;             // Error: Matrix<>::operator=() = delete;
mcopy(B, &A);      // In-place copy; verifies dimensions; no alloc/dealloc
Matrix<T> A(B);    // Copy semantic; allocates A
A = std::move(B);  // Move semantic; deallocates A; B is empty
```

# Hardware Accelerators

The Matrix library is templated with the BLAS enum.
```
// BLAS Libraries
enum BLAS { REF, ACC, OPB, MKL };
Matrix<BLAS> A;
```

The enumerated types correspond to a hardware accelerator library with the exception of the reference implementation.
1. REF : Reference Implementation
1. ACC : Apple Accelerate Framework
1. OPB : OpenBLAS
1. MKL : Intel's Math Kernel Library

## Deleted Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| `A = B;`                 | [COPY]         |

## Memory-Allocating Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| `Matrix<T> A(m, n);`     | [ALLOCATE]
| `Matrix<T> A(B);`        | [COPY]         |
| `C = A * B;`             | [MULTIPLY]     |

## Allocation Moving Operations:

| Syntax                       | Operation      |
| ---------------------------  | -------------- |
| `Matrix<T> A(std::move(B));` | [MOVE B -> A]  |
| `A = std::move(B);`          | [MOVE B -> A]  |
| `B = alpha * A;`             | [SCALAR MULT] [MOVE A -> B] |
| `C = std::move(A) + B;`      | [ADD] [MOVE A -> C] |
| `C = A + std::move(B);`      | [ADD] [MOVE B -> C] |
| `C = std::move(A) - B;`      | [SUBTRACT] [MOVE A -> C] |
| `C = A - std::move(B);`      | [SUBTRACT] [MOVE B -> C] |

## In Place Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| `mcopy(A, &B)`           | [COPY A -> B]  |
| `A += B;`                | [ADD]          |
| `A -= B;`                | [SUBTRACT]     |

# Contributing

PRs submitted to https://www.github.com/ccmagruder/Matrix.git are welcome.
The project is cpplint and cppcheck compliant.
To enable linting configure cmake with `cmake .. -DLINT=ON`.
