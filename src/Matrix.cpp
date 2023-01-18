// Copyright 2023 Caleb Magruder

#include "Matrix.h"

std::ostream& operator<<(std::ostream& os, BLAS type) {
    switch (type) {
        case REF:
            os << "REF";
            break;
        case ACC:
            os << "ACC";
            break;
        case OPB:
            os << "OPB";
            break;
        case MKL:
            os << "MKL";
            break;
    }
    return os;
}
