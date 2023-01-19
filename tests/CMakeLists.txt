# Copyright 2023 Caleb Magruder

project(Matrix)

include(GoogleTest)

configure_file(tMatrix.cpp.in tMatrix.cpp)

add_executable(tMatrix tMatrix.cpp)

target_include_directories(tMatrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tMatrix Matrix)

add_test(NAME tMatrix
         WORKING_DIRECTORY tests
         COMMAND tMatrix)
