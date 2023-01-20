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

To compile with the hardware-accelerator backends append `-DUSE_ACC=ON` to enable Apple's Accelerate Framework, `-DUSE_MKL=ON` to enable Intel's Math Kernel Library, and `-DUSE_OPB=ON` to enable OpenBLAS. On MacOS with an Intel architecture it is possible to configure cmake with all three backends simultaneously.
```
cmake .. -DUSE_ACC=ON -DUSE_MKL=ON -DUSE_OPB=ON
```

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
