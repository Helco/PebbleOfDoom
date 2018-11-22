#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"

struct Renderer
{
    const Level* level;
    Location location;
    real_t  fovStuff, halfFov;
    lineSeg_t leftFovSeg, rightFovSeg;

    TextureManagerHandle textureManager;
};

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);

#endif
