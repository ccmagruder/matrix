#!/bin/bash

if [ -d "build.release" ]; then rm -Rf build.release; fi

mkdir build.release

cmake -B build.release -DCMAKE_BUILD_TYPE=Release .

cmake --build build.release

build.release/benchmark


