# PebbleOfDoom

```
Please remember to clone this repository with the parameter `--recurse-submodules` or use `git submodule update --init --recursive` to download dependencies after cloning
```

## PCMockup build instructions

In any case you need a recent version of CMake and the SDL2 library (prebuilt) on your computer. It is much recommended that you create a `build` folder and then run `cmake ..` in your favorite terminal.

### With Visual Studio

For Visual Studio you have to download the *VC* development library of SDL2 at [https://libsdl.org], also you might have to tell cmake where to find your downloaded (and extracted) SDL2 library by inserting the parameter `-DSDL2_PATH=<path-to-SDL2>`.
