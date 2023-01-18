### Matrix

Matrix Algebra (addition, multiplication, tranpose) hardware accelerated via Intel's Math Kernel Library (MKL), specifically the Basic Linear Algebra Subprograms (BLAS) and Vector Mathematics Library (VML). Copy / Move symmantics prevent accidental copies. Reference implementation MatrixRef is included for unit testing and benchmarking MKL.

The Matrix library is built with copy / move symantics to prevent accidental matrix copies. The assignment operator is deleted, making the only possible way to copy a matrix is via constructor Matrix(A). The following operations are valid on types Matrix(MatrixMKL) and MatrixRef:

#### Deleted Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| A = B;                   | [COPY]         |

#### Memory-Allocating Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| Matrix A(m,n);           | [ALLOCATE]
| Matrix A(B);             | [COPY]         |
| C = A*B;                 | [MULTIPLY]     |

#### Allocation Moving Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| Matrix A(std::move(B));  | [MOVE B -> A]  |
| A = std::move(B);        | [MOVE B -> A]  |
| B = alpha*A;             | [SCALAR MULT] [MOVE A -> B] |
| C = A+B;                 | [ADD] [MOVE A -> C] |
| C = A-B;                 | [SUBTRACT] [MOVE A -> C] |

#### In Place Operations:

| Syntax                   | Operation      |
| ------------------------ | -------------- |
| A+=B;                    | [ADD]          |
| A-=B;                    | [SUBTRACT]     |

