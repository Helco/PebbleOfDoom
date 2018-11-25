#!/bin/bash
set -e
if [ ! -d "build" ]
then
    mkdir build
fi
cd build

wget http://libsdl.org/release/SDL2-devel-2.0.9-VC.zip
7z x SDL2-devel-2.0.9-VC.zip

wget https://marketplace.visualstudio.com/_apis/public/gallery/publishers/LLVMExtensions/vsextensions/llvm-toolchain/latest/vspackage
wget https://github.com/Microsoft/vsixbootstrapper/releases/download/1.0.31/VSIXBootstrapper.exe
VSIXBootstrapper.exe /q llvm.vsix

export POD_CMAKE_ARGS="${POD_CMAKE_ARGS} -DSDL2_PATH=`cmd //c cd`\\SDL2-2.0.9 -T llvm"

cd ..
