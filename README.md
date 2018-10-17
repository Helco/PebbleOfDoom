# PebbleOfDoom

```
Please remember to clone this repository with the parameter `--recurse-submodules` or use `git submodule update --init --recursive` to download dependencies after cloning
```

## PCMockup build instructions

In any case you need a recent version of CMake and the SDL2 library (prebuilt) on your computer. It is much recommended that you create a `build` folder and then run `cmake ..` in your favorite terminal.

### With Visual Studio

For Visual Studio you have to download the *VC* development library of SDL2 at [https://libsdl.org], also you might have to tell cmake where to find your downloaded (and extracted) SDL2 library by inserting the parameter `-DSDL2_PATH=<path-to-SDL2>`.

### With make/gcc

As CMake is generating release makefiles as default you might want to add the parameter `-DCMAKE_BUILD_TYPE=Debug`. With this configuration you can install SDL2 at toolchain/system level and don't have to declare the location of it.
