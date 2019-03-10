#!/bin/bash
set -e
mkdir -p build
cd build

docker exec -it emscripten bash -c "cd build && emconfigure cmake .."
docker exec -it emscripten bash -c "cd build && cmake --build ."

