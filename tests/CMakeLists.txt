# Copyright 2023 Caleb Magruder

project(Matrix)

include(GoogleTest)

configure_file(tMatrix.cpp.in tMatrix.cpp)

add_executable(tMatrix tMatrix.cpp)

add_library(TestWithLogging SHARED ${CMAKE_SOURCE_DIR}/tests/TestWithLogging.cpp
                                   ${CMAKE_SOURCE_DIR}/tests/Logger.cpp)
target_include_directories(TestWithLogging PUBLIC ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(TestWithLogging gtest gtest_main)

target_link_libraries(tMatrix Matrix TestWithLogging)

add_test(NAME tMatrix
         WORKING_DIRECTORY tests
         COMMAND tMatrix)
