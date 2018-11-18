#!/bin/bash
set -e
echo "Building Pebble project..."
cd "${TRAVIS_BUILD_DIR}"
# pebble build isn't used because it fails
yes | ~/pebble-dev/${PEBBLE_SDK}/bin/pebble build
