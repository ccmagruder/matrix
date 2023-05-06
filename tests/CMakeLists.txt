# Copyright 2023 Caleb Magruder

project(Matrix)

find_package(GTest REQUIRED)

include(GoogleTest)

add_library(Test SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/TestWithLogging.cpp)

target_include_directories(Test PUBLIC ${CMAKE_SOURCE_DIR}/include
                                       ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Test GTest::gtest GTest::gtest_main)

configure_file(tMatrix.cpp.in tMatrix.cpp)

add_executable(tMatrix tMatrix.cpp)

target_link_libraries(tMatrix Matrix Test)

add_test(NAME tMatrix
         WORKING_DIRECTORY tests
         COMMAND tMatrix)
