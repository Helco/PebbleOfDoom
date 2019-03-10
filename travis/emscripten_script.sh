#!/bin/bash
set -e
cd PebbleOfDoom
mkdir -p build
cd build

docker exec -it emscripten cmake ..
cmake --build .
