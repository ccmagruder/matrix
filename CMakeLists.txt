# Copyright 2023 Caleb Magruder

cmake_minimum_required(VERSION 3.22)

project(Matrix)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_BUILD_TYPE Debug)

option(LINT "Run cpplint and cppcheck linting" OFF)

option(USE_ACC "BLAS: Apple's Accelerate Framework" OFF)

option(USE_OPB "BLAS: OpenBLAS Library" OFF)

option(USE_MKL "BLAS: Intel's Math Kernel Library" OFF)

if (${LINT})
    # Directory build/ does not need to be linted
    set(CMAKE_CXX_CPPLINT "cpplint;--quiet;--exclude=${CMAKE_BINARY_DIR}/*")
    set(CMAKE_CXX_CPPCHECK "cppcheck;--quiet;--suppress=*:${CMAKE_BINARY_DIR}/*")
endif (${LINT})

add_subdirectory(extern)
add_subdirectory(tests)

add_library(Matrix SHARED ${CMAKE_SOURCE_DIR}/src/Matrix.cpp)

target_include_directories(Matrix PUBLIC
                           ${CMAKE_SOURCE_DIR}/include)

##############################  BLAS: Accelerate  #############################

if (${USE_ACC})
    find_library(ACC Accelerate)
    add_library(MatrixACC OBJECT ${CMAKE_SOURCE_DIR}/src/MatrixACC.cpp)
    target_include_directories(MatrixACC PUBLIC ${CMAKE_SOURCE_DIR}/include
                                                ${ACC}/Headers)
    target_link_libraries(Matrix $<TARGET_OBJECTS:MatrixACC>
                                        "-framework Accelerate")
endif ()

enable_testing()
