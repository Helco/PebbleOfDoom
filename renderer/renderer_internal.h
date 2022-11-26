#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"
#include <stdbool.h>

#define MAX_DRAW_DEPTH 8
#define MAX_DRAW_SECTORS 96

typedef struct BoundarySet {
    uint8_t yTop[RENDERER_WIDTH];
    uint8_t yBottom[RENDERER_WIDTH];
} BoundarySet;

typedef struct DrawRequest {
    const Sector* sector;
    const Sector* sourceSector;
    int left, right, depth;
} DrawRequest;

typedef struct DrawRequestStack {
    DrawRequest requests[MAX_DRAW_SECTORS];
    int count;
} DrawRequestStack;

struct Renderer
{
    const Level* level;
    xz_t* transformedVertices;
    uint32_t* transformedStatus;

    Location location;
    real_t eyeHeight;
    real_t horFovScale, verFovScale, fov;
    lineSeg_t leftFovSeg, rightFovSeg;
    BoundarySet stackBoundarySets[MAX_DRAW_DEPTH];
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
void drawRequestStack_push(DrawRequestStack* stack, const DrawRequest* source, const Sector* sector, int left, int right);

void bresenham_init(BresenhamIterator* it, int x0, int y0, int x1, int y1);
BresenhamStep bresenham_step(BresenhamIterator* it);
BresenhamStep bresenham_fasterYStep(BresenhamIterator* it);

void renderer_setFieldOfView(Renderer* me, real_t newFov);
void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);

#endif
