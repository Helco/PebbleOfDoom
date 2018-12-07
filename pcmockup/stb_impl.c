// All implementations of stb

#define _CRT_SECURE_NO_WARNINGS // to quiet msvc

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#elif defined __clang__
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#define STB_DEFINE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_NO_SIMD // SIMD build crashes clang on windows
#include "stb.include.h"
