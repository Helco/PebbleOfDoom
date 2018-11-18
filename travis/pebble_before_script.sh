#!/bin/bash
# Install NPM 3
npm install npm@3 -g

set -e
echo 'pBuild 1.6 - fork by Helco'
echo 'Installing Pebble SDK and its Dependencies...'

cd ~

mkdir -p ~/pebble-dev
mkdir -p ~/.pebble-sdk
touch ~/.pebble-sdk/NO_TRACKING

export PEBBLE_SDK=pebble-sdk-4.5-linux64

# Get the Pebble SDK and toolchain
if [ ! -d "$HOME/pebble-dev/pebble-tool" ]; then
  wget https://developer.rebble.io/s3.amazonaws.com/assets.getpebble.com/pebble-tool/${PEBBLE_SDK}.tar.bz2

  # Extract the SDK
  tar -jxf ${PEBBLE_SDK}.tar.bz2 -C ~/pebble-dev/ --strip-components 1

  # Install the Python library dependencies locally
  cd ~/pebble-dev/
  virtualenv --no-site-packages .env
  source .env/bin/activate
  pip install -r requirements.txt
  deactivate
fi

if [ ! -d "$HOME/.pebble-sdk/SDKs/4.3" ]; then
  yes | ${HOME}/pebble-dev/bin/pebble sdk install https://github.com/aveao/PebbleArchive/raw/master/SDKCores/sdk-core-4.3.tar.bz2
fi
