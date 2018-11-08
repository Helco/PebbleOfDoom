#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"

typedef struct Wall
{
    xz_t start, end;
    real_t height, heightOffset;
    GColor wallColor, floorColor, ceilColor;
} Wall;

struct Renderer
{
    Wall wall, wall2, wall3;

    xz_t pos;
    real_t height;
    real_t angle, halfFov, fovStuff;
    lineSeg_t leftFovSeg, rightFovSeg;
};

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);

#endif
