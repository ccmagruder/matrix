# Copyright 2023 Caleb Magruder

cmake_minimum_required(VERSION 3.21)

project(Matrix)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

###############################################################################
############################  Configure Options  ##############################
###############################################################################

option(LINT "Run cpplint and cppcheck linting" OFF)

if (${LINT})
    # Directory build/ does not need to be linted
    set(CMAKE_CXX_CPPLINT "cpplint;--quiet;--exclude=${CMAKE_BINARY_DIR}/*")
    set(CMAKE_CXX_CPPCHECK "cppcheck;--quiet;--suppress=*:${CMAKE_BINARY_DIR}/*")
endif (${LINT})

###############################################################################
###############################  Matrix Library  ##############################
###############################################################################

add_library(Matrix SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/Matrix.cpp)

target_include_directories(Matrix PUBLIC ${CMAKE_SOURCE_DIR}/include)

##############################  BLAS: Accelerate  #############################

find_library(ACC Accelerate)
if (ACC)
    add_library(MatrixACC OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixACC.cpp)
    target_include_directories(MatrixACC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                ${ACC}/Headers)
    target_link_libraries(Matrix $<TARGET_OBJECTS:MatrixACC>
                                        "-framework Accelerate")
    target_compile_definitions(Matrix PUBLIC ACC_FOUND)
endif ()

#################################  BLAS: MKL  #################################

find_package(MKL QUIET)
if (MKL)
    find_library(MKL mkl_rt HINTS ${MKL_ROOT}/lib)
    add_library(MatrixMKL OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixMKL.cpp)
    target_include_directories(MatrixMKL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                ${MKL_ROOT}/include)
    target_link_libraries(Matrix $<TARGET_OBJECTS:MatrixMKL>
        ${MKL}
        ${MKL_THREAD_LIB})
    target_compile_definitions(Matrix PUBLIC MKL_FOUND)
endif ()

###############################  BLAS: OpenBLAS  ##############################

find_library(OPB openblas HINTS /opt/homebrew/opt/openblas/lib
                                  /usr/local/opt/openblas/lib)
if (OPB)
    add_library(MatrixOPB OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixOPB.cpp)
    target_include_directories(MatrixOPB PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                /opt/homebrew/opt/openblas/include
                                                /usr/local/opt/openblas/include)
    target_link_libraries(Matrix $<TARGET_OBJECTS:MatrixOPB>
                              ${OPB})
    target_compile_definitions(Matrix PUBLIC OPB_FOUND)
endif ()

###############################################################################
##################################  Tests  ####################################
###############################################################################

# If submodule, skip tests and google test dependency.
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    add_subdirectory(tests)
endif()

###############################################################################
#############################  Benchmark Profiler  ############################
###############################################################################

find_package(benchmark REQUIRED)

add_executable(benchmark ${CMAKE_SOURCE_DIR}/src/benchmark.cpp)

target_link_libraries(benchmark Matrix benchmark::benchmark benchmark::benchmark_main)

enable_testing()
