#ifndef RENDERER_INTERNAL_H
#define RENDERER_INTERNAL_H
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DRAW_DEPTH 16

typedef struct DrawRequest {
    const Sector* sector;
    const Sector* sourceSector;
    int left, right;
} DrawRequest;

typedef struct DrawRequestStack {
    DrawRequest requests[MAX_DRAW_DEPTH];
    int start, end;
} DrawRequestStack;

struct Renderer
{
    const Level* level;
    Location location;
    real_t eyeHeight;
    real_t fovStuff, halfFov;
    lineSeg_t leftFovSeg, rightFovSeg;
    int yTop[RENDERER_WIDTH], yBottom[RENDERER_WIDTH];
    DrawRequestStack drawRequests;

    TextureManagerHandle textureManager;
};

void drawRequestStack_reset(DrawRequestStack* stack);
void drawRequestStack_push(DrawRequestStack* stack, const Sector* sector, int left, int right, const Sector* sourceSector);
const DrawRequest* drawRequestStack_pop(DrawRequestStack* stack);

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result);

#ifdef __cplusplus
}
#endif

#endif
