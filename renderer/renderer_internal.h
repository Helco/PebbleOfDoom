#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"

typedef struct Wall
{
    xz_t start, end;
    real_t height;
    GColor wallColor, floorColor, ceilColor;
} Wall;

struct Renderer
{
    Wall wall, wall2, wall3;

    xz_t pos;
    real_t angle, halfFov, fovStuff;
    lineSeg_t leftFovSeg, rightFovSeg;
};

#endif