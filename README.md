# PebbleOfDoom [![Build Status](https://travis-ci.com/Helco/PebbleOfDoom.svg?token=ppZoE6wRQrszEciSQ3hJ&branch=master)](https://travis-ci.com/Helco/PebbleOfDoom)

```
Please remember to clone this repository with the parameter `--recurse-submodules` or use `git submodule update --init --recursive` to download dependencies after cloning
```

## Coordinate system

This project uses the left-hand y-up coordinate system for both screen- and world-space, thus it looks like this:

```
y
^
|   z
| /
|/
+--------> x
```

## PCMockup build instructions

In any case you need a recent version of CMake and the SDL2 library (prebuilt) on your computer. It is much recommended that you create a `build` folder and then run `cmake ..` in your favorite terminal.

### With Visual Studio

For Visual Studio you have to download the *VC* development library of SDL2 at [https://libsdl.org], also you might have to tell cmake where to find your downloaded (and extracted) SDL2 library by inserting the parameter `-DSDL2_PATH=<path-to-SDL2>`.

CAUTION: If cmake could not find your SDL2 path you might have to delete the contents of your build folder before trying again, otherwise even correct cmake launches *may not work*!

Further more this is a C11 project, as such the MSVC compiler will not work! To use Visual Studio you have to install [Clang/LLVM](http://releases.llvm.org/download.html) and the [LLVM Toolchain Extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain). Also you have to add the parameter `-T llvm` to your CMake call.

As such a typical CMake call on Windows to develop this project looks like this:

```
cmake -DSDL2_PATH="C:/libs/SDL2-2.0.5/" -T llvm ..
```

You might have to copy your `SDL.dll` in the `build/Debug` or `build/Release` directory to start pcmockup.

### With make/gcc

As CMake is generating release makefiles as default you might want to add the parameter `-DCMAKE_BUILD_TYPE=Debug`. With this configuration you can install SDL2 at toolchain/system level and don't have to declare the location of it.

## Run unit tests

### With Visual Studio

In Visual Studio it is easiest to use the Test Explorer (Test->Windows->TestExplorer) and the GoogleTest Adapter Extension (Tools->Extensions and Updates). Then every test application should be discovered automatically and you can run and debug tests from there.

### With Visual Studio Code

For VSCode there exists the "Google Test Explorer for Visual Studio Code" extension which provides a similar service to the Test Explorer in Visual Studio.

For this extension to find the gtest applications you have to declare them in the workspace settings.

### With Terminal/CI

When run via a terminal, just go into your `build` folder and run `ctest`.
