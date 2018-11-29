#include "renderer_internal.h"
#include "algebra.h"
#include <assert.h>

Renderer* renderer_init()
{
    Renderer* me = (Renderer*)malloc(sizeof(Renderer));
    if (me == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate renderer");
        return NULL;
    }
    memset(me, 0, sizeof(Renderer));

    me->halfFov = real_degToRad(real_from_int(30));

    xz_t nearPlane, farPlane;
    nearPlane.z = real_from_float(1.0f);
    farPlane.z = real_from_int(500);

    const real_t tanHalfFov = real_tan(me->halfFov);
    const real_t minus_one = real_from_int(-1);
    nearPlane.x = real_mul(tanHalfFov, nearPlane.z);
    farPlane.x = real_mul(tanHalfFov, farPlane.z);
    me->leftFovSeg.start.xz = xz(real_mul(minus_one, nearPlane.x), nearPlane.z);
    me->leftFovSeg.end.xz = xz(real_mul(minus_one, farPlane.x), farPlane.z);
    me->rightFovSeg.start.xz = nearPlane;
    me->rightFovSeg.end.xz = farPlane;
    me->fovStuff = real_div(real_from_int(-HALF_RENDERER_WIDTH), tanHalfFov);
    me->eyeHeight = real_from_int(12);

    return me;
}

void renderer_free(Renderer* me)
{
    if (me == NULL)
        return;
    free(me);
}

void renderer_setLevel(Renderer* renderer, const Level* level)
{
    renderer->level = level;
    renderer->location = level->playerStart;
}

void renderer_setTextureManager(Renderer* me, TextureManagerHandle handle)
{
    me->textureManager = handle;
}

static xz_t myxz_rotate(xz_t a, real_t angleInRad)
{
    real_t s = real_sin(angleInRad);
    real_t c = real_cos(angleInRad);
    return xz(
        real_sub(real_mul(a.x, s), real_mul(a.z, c)),
        real_add(real_mul(a.x, c), real_mul(a.z, s))
    );
}

xz_t renderer_transformVector(const Renderer* me, xz_t vector)
{
    return myxz_rotate(vector, me->location.angle);
}

xz_t renderer_transformPoint(const Renderer* me, xz_t point)
{
    return renderer_transformVector(me, xz_sub(point, me->location.position));
}

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result)
{
    result->start.xz = renderer_transformPoint(me, line->start.xz);
    result->end.xz = renderer_transformPoint(me, line->end.xz);
}

void renderer_transformWall(const Renderer* me, const Sector* sector, int wallIndex, lineSeg_t* result)
{
    lineSeg_t lineSeg;
    lineSeg.start.xz = sector->walls[wallIndex].startCorner;
    lineSeg.end.xz = sector->walls[(wallIndex + 1) % sector->wallCount].startCorner;
    renderer_transformLine(me, &lineSeg, result);
}

typedef struct
{
    struct {
        int x;
        int yStart, yEnd;
    } left, right;
} WallSection;

bool_t renderer_clipByFov(const Renderer* me, lineSeg_t* wallSeg)
{
    xz_t leftIntersection, rightIntersection;
    bool_t intersectsLeft = xz_lineIntersect(*wallSeg, me->leftFovSeg, &leftIntersection);
    bool_t intersectsRight = xz_lineIntersect(*wallSeg, me->rightFovSeg, &rightIntersection);
    real_t wallPhaseLeft = intersectsLeft ? xz_linePhase(*wallSeg, leftIntersection) : real_zero;
    real_t wallPhaseRight = intersectsRight ? xz_linePhase(*wallSeg, rightIntersection) : real_zero;
    bool_t inWallSegLeft = real_inBetween(wallPhaseLeft, real_zero, real_one);
    bool_t inWallSegRight = real_inBetween(wallPhaseRight, real_zero, real_one);
    bool_t result = true;

    if (real_compare(wallSeg->start.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        wallSeg->start.xz = (real_compare(rightIntersection.z, real_zero) > 0 && inWallSegRight)
            ? rightIntersection
            : inWallSegLeft ? leftIntersection : (result = false, xz_zero);
    }

    if (real_compare(wallSeg->end.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        wallSeg->end.xz = (real_compare(leftIntersection.z, real_zero) > 0 && inWallSegLeft)
            ? leftIntersection
            : inWallSegRight ? rightIntersection : (result = false, xz_zero);
    }

    return result;
}

void renderer_project(const Renderer* me, const Sector* sector, const lineSeg_t* transformedSeg, WallSection* projected)
{
    //const real_t halfHeight = real_div(real_from_int(sector->height), real_from_int(2));
    const real_t relHeightOffset = real_sub(real_from_int(sector->heightOffset), real_add(me->location.height, me->eyeHeight));
#define scale_height(value) (real_mul(real_from_int(HALF_RENDERER_HEIGHT), (value)))
    const real_t scaledWallHeight =    scale_height(real_add(real_from_int(sector->height), relHeightOffset));
    const real_t negScaledWallHeight = scale_height(relHeightOffset);
#undef scale_height
    const xz_t startT = transformedSeg->start.xz;
    const xz_t endT = transformedSeg->end.xz;

#define div_and_int(value, z) (real_to_int(real_div((value), (z))))
    real_t projectedLeftX =   real_sub(real_div(real_mul(startT.x, me->fovStuff), startT.z), real_from_float(0.5f)); // -0.5 fixes rounding error
    projected->left.x =       real_to_int(projectedLeftX)                             + HALF_RENDERER_WIDTH;
    projected->left.yStart =  div_and_int(negScaledWallHeight, startT.z)              + HALF_RENDERER_HEIGHT;
    projected->left.yEnd =    div_and_int(scaledWallHeight, startT.z)                 + HALF_RENDERER_HEIGHT;

    projected->right.x =      div_and_int(real_mul(endT.x, me->fovStuff), endT.z)     + HALF_RENDERER_WIDTH;
    projected->right.yStart = div_and_int(negScaledWallHeight, endT.z)                + HALF_RENDERER_HEIGHT;
    projected->right.yEnd =   div_and_int(scaledWallHeight, endT.z)                   + HALF_RENDERER_HEIGHT;
#undef div_and_int
}

void renderer_renderWall(Renderer* me, GColor* framebuffer, const DrawRequest* request, int wallIndex)
{
    const Sector* const sector = request->sector;
    const Wall* const wall = &sector->walls[wallIndex];
    const real_t nearPlane = me->leftFovSeg.start.xz.z;
    lineSeg_t wallSeg;
    renderer_transformWall(me, sector, wallIndex, &wallSeg);
    if (real_compare(wallSeg.start.xz.z, nearPlane) < 0 && real_compare(wallSeg.end.xz.z, nearPlane) < 0)
        return;

    if (!renderer_clipByFov(me, &wallSeg))
        return;

    WallSection p;
    renderer_project(me, sector, &wallSeg, &p);
    if (p.left.x >= p.right.x || p.right.x < request->left || p.left.x > request->right)
        return;

    const Sector* targetSector = &me->level->sectors[wall->portalTo];
    int portalNomStart, portalNomEnd;
    if (wall->portalTo >= 0 && targetSector != request->sourceSector) {
        portalNomStart = clampi(0, targetSector->heightOffset - sector->heightOffset, sector->height);
        portalNomEnd = clampi(0, portalNomStart + targetSector->height, sector->height);
        //if (sector->wallCount == 3)
        drawRequestStack_push(&me->drawRequests, targetSector,
            max(request->left, p.left.x), min(request->right, p.right.x), sector);
    }

    // render wall
    for (int x = max(request->left, p.left.x); x <= min(request->right, p.right.x); x++) {
        const int yBottom = me->yBottom[x];
        const int yTop = me->yTop[x];
        GColor* curPixel = framebuffer + x * RENDERER_HEIGHT + yBottom;
        int yCurStart = lerpi(x, p.left.x, p.right.x, p.left.yStart, p.right.yStart);
        int yCurEnd = lerpi(x, p.left.x, p.right.x, p.left.yEnd, p.right.yEnd);
        if (yCurEnd < yBottom || yCurStart > yTop || yCurStart >= yCurEnd)
            continue;

        int yPortalStart = yCurEnd, yPortalEnd = yTop + 1;
        if (wall->portalTo >= 0) {
            me->yBottom[x] = yPortalStart = clampi(yBottom, lerpi(portalNomStart, 0, sector->height, yCurStart, yCurEnd), yTop);
            me->yTop[x] = yPortalEnd = clampi(yBottom, lerpi(portalNomEnd, 0, sector->height, yCurStart, yCurEnd), yTop);
        }

        int y;
        for (y = yBottom; y < max(yBottom, yCurStart); y++)
            *(curPixel++) = sector->floorColor;
        for (; y <= min(yTop, yPortalStart - 1); y++)
            *(curPixel++) = wall->color;
        if (wall->portalTo >= 0) {
            //for (; y <= min(yTop, yPortalEnd); y++)
                //*(curPixel++) = ((y / 4) % 2) ? GColorFromRGB(255, 0, 255) : GColorFromRGB(0, 0, 0);
            curPixel += yPortalEnd - y;
            for (y = yPortalEnd; y <= min(yTop, yCurEnd); y++)
                *(curPixel++) = wall->color;
        }
        for (; y <= yTop; y++)
            *(curPixel++) = sector->ceilColor;
    }
}

void renderer_renderSector(Renderer* renderer, GColor* framebuffer, const DrawRequest* request)
{
    for (int i = 0; i < request->sector->wallCount; i++)
        renderer_renderWall(renderer, framebuffer, request, i);
}

void renderer_moveLocation(Renderer* renderer, xz_t xz)
{
    xz = xz_rotate(xz, renderer->location.angle);
    renderer->location.position = xz_add(renderer->location.position, xz);
}

void renderer_render(Renderer* renderer, GColor* framebuffer)
{
    if (renderer->level == NULL)
        return;
    memset(framebuffer, 0, RENDERER_WIDTH * RENDERER_HEIGHT);
    memset(renderer->yBottom, 0, sizeof(renderer->yBottom));
    for (int i = 0; i < RENDERER_WIDTH; i++)
        renderer->yTop[i] = RENDERER_HEIGHT - 1;
    drawRequestStack_reset(&renderer->drawRequests);
    drawRequestStack_push(&renderer->drawRequests,
        &renderer->level->sectors[renderer->location.sector],
        0, RENDERER_WIDTH - 1, NULL);

    const DrawRequest* curRequest = drawRequestStack_pop(&renderer->drawRequests);
    while (curRequest != NULL)
    {
        renderer_renderSector(renderer, framebuffer, curRequest);
        curRequest = drawRequestStack_pop(&renderer->drawRequests);
    }
};

void renderer_rotateRight(Renderer* renderer)
{
    renderer->location.angle = real_add(renderer->location.angle, real_degToRad(real_from_int(1)));
}

void renderer_rotateLeft(Renderer* renderer)
{
    renderer->location.angle = real_sub(renderer->location.angle, real_degToRad(real_from_int(1)));
}

void renderer_moveForward(Renderer* renderer)
{
    renderer_moveLocation(renderer, xz(real_one, real_zero));
}

void renderer_moveBackwards(Renderer* renderer)
{
    renderer_moveLocation(renderer, xz(real_neg(real_one), real_zero));
}

void renderer_moveRight(Renderer* renderer)
{
    renderer_moveLocation(renderer, xz(real_zero, real_one));
}

void renderer_moveLeft(Renderer* renderer)
{
    renderer_moveLocation(renderer, xz(real_zero, real_neg(real_one)));
}

void renderer_moveUp(Renderer* renderer)
{
    renderer->location.height = real_add(renderer->location.height, real_one);
}

void renderer_moveDown(Renderer* renderer)
{
    renderer->location.height = real_sub(renderer->location.height, real_one);
}

void renderer_moveTo(Renderer* renderer, Location relativOrigin)
{
    renderer->location = relativOrigin;
}

void drawRequestStack_reset(DrawRequestStack* stack)
{
    stack->start = stack->end = 0;
}

void drawRequestStack_push(DrawRequestStack* stack, const Sector* sector, int left, int right, const Sector* sourceSector)
{
    const int insertAt = stack->end;
    stack->end = (stack->end + 1) % MAX_DRAW_DEPTH;
    assert(stack->end != stack->start);
    stack->requests[insertAt] = (DrawRequest) {
        .sector = sector,
        .left = left,
        .right = right,
        .sourceSector = sourceSector
    };
}

const DrawRequest* drawRequestStack_pop(DrawRequestStack* stack)
{
    if (stack->start == stack->end)
        return NULL;
    const DrawRequest* result = &stack->requests[stack->start];
    stack->start = (stack->start + 1) % MAX_DRAW_DEPTH;
    return result;
}
