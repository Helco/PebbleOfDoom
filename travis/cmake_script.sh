#!/bin/bash
set -e
if [ ! -d "build" ]
then
    mkdir build
fi
cd build

cmake ${POD_CMAKE_ARGS} ..
cmake --build .
ctest
