#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"
#include <stdbool.h>

#define MAX_DRAW_DEPTH 16

typedef struct DrawRequest {
    const Sector* sector;
    const Sector* sourceSector;
    int left, right, depth;
} DrawRequest;

typedef struct DrawRequestStack {
    DrawRequest requests[MAX_DRAW_DEPTH];
    int start, end, depth;
} DrawRequestStack;

typedef struct BoundarySet {
    short yTop[RENDERER_WIDTH], yBottom[RENDERER_WIDTH];
} BoundarySet;

struct Renderer
{
    const Level* level;
    Location location;
    real_t eyeHeight;
    real_t horFovScale, fov;
    lineSeg_t leftFovSeg, rightFovSeg;
    BoundarySet boundarySets[2];
    BoundarySet wallBoundaries;
    short spanStart[RENDERER_HEIGHT];
    int curBoundarySet;
    DrawRequestStack drawRequests;

    TextureManagerHandle textureManager;
};

typedef struct BresenhamIterator
{
    int x0, y0, x1, y1;
    int dx, dy, sx, sy, err;
    bool isFirstStep;
} BresenhamIterator;
typedef enum BresenhamStep
{
    BRESENHAMSTEP_INIT = 0,
    BRESENHAMSTEP_X,
    BRESENHAMSTEP_Y,
    BRESENHAMSTEP_NONE
} BresenhamStep;

void drawRequestStack_reset(DrawRequestStack* stack);
void drawRequestStack_push(DrawRequestStack* stack, const Sector* sector, int left, int right, const Sector* sourceSector);
void drawRequestStack_nextDepth(DrawRequestStack* stack);
const DrawRequest* drawRequestStack_pop(DrawRequestStack* stack);

void bresenham_init(BresenhamIterator* it, int x0, int y0, int x1, int y1);
BresenhamStep bresenham_step(BresenhamIterator* it);

void renderer_setFieldOfView(Renderer* me, real_t newFov);
void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);

#endif
