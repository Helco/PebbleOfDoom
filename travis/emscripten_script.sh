#!/bin/bash
set -e
mkdir -p build
cd build

docker exec -it emscripten cmake ..
cmake --build .
