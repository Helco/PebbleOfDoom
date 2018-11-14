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

    PlayerLocation playerLocation;
    real_t  fovStuff, halfFov;
    lineSeg_t leftFovSeg, rightFovSeg;
};

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);
void renderer_movePlayerInPlayerSpace(Renderer* me, xz_t xz);

#endif
