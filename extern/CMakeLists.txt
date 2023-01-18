# Copyright 2023 Caleb Magruder

project(Matrix)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/extern)

# Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)

set(BUILD_GMOCK OFF)

FetchContent_MakeAvailable(googletest)
