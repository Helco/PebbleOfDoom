#include "texgen_internal.h"
#include <stdbool.h>

#define MAX_NOISE 32

typedef struct TexGen_Bricks_Params {
    int brickWidth, brickHeight, brickNoise;
    GColor brickColor;
    int borderSize, borderWalls, borderNoise;
    GColor borderColor;
} TexGen_Bricks_Params;

static const TexGen_Bricks_Params texGen_bricks_defaultParams = {
    .brickWidth = 8,
    .brickHeight = 4,
    .brickNoise = 20,
    .brickColor = { .r = 3, .a = 3},
    .borderSize = 1,
    .borderWalls = 0b1010,
    .borderNoise = 0,
    .borderColor = { .r = 1, .g = 1, .b = 1, .a = 3 }
};

static GColor bricks_applyNoise(GColor base, int noise) {
    if (noise == 0 || rand() % (MAX_NOISE - noise))
        return base;

    const int delta = rand() % 2 ? 1 : -1;
    base.r = clampi(0, base.r + delta, 3);
    base.g = clampi(0, base.g + delta, 3);
    base.b = clampi(0, base.b + delta, 3);
    return base;
}

bool_t texGen_bricks_execute(GColor* output, int logSize, const void* rawParams)
{
    const TexGen_Bricks_Params* params = (const TexGen_Bricks_Params*)rawParams;
    const int size = 1 << logSize;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            const int xOffset = (y / params->brickHeight) % 2 ? 0 : params->brickWidth / 2;
            const int bx = (x + xOffset) % params->brickWidth;
            const int by = y % params->brickHeight;
            const bool isBorder =
                ((params->borderWalls & 1) && bx < params->borderSize) ||
                ((params->borderWalls & 2) && (params->brickWidth - bx) <= params->borderSize) ||
                ((params->borderWalls & 4) && by < params->borderSize) ||
                ((params->borderWalls & 8) && (params->brickHeight - by) <= params->borderSize);
            *(output++) = bricks_applyNoise(
                isBorder ? params->borderColor : params->brickColor,
                isBorder ? params->borderNoise : params->brickNoise);
        }
    }
    return true;
}

BEGIN_TEXGENERATOR(Bricks, TexGen_Bricks_Params)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BrickWidth,
        "Width of a single brick",
        brickWidth, 1, 128)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BrickHeight,
        "Height of a single brick",
        brickHeight, 1, 128)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BrickNoise,
        "Noise level of bricks",
        brickNoise, 0, MAX_NOISE)
    TEXGENPARAM_COLOR(
        TexGenParam_Bricks_BrickColor,
        "Color of bricks",
        brickColor)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BorderSize,
        "Size of border",
        borderSize, 1, 64)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BorderWalls,
        "Enabled border walls",
        borderWalls, 0, 15)
    TEXGENPARAM_INT(
        TexGenParam_Bricks_BorderNoise,
        "Noise level of border",
        borderNoise, 0, MAX_NOISE)
    TEXGENPARAM_COLOR(
        TexGenParam_Bricks_BorderColor,
        "Color of border",
        borderColor)
END_TEXGENERATOR(Bricks,
    TexGenerator_Bricks,
    "Brick wall textures",
    texGen_bricks_defaultParams,
    texGen_bricks_execute)
