#include "renderer_internal.h"
#include "algebra.h"

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

xz_t renderer_transformVector(const Renderer* me, xz_t vector)
{
    vector = xz(real_neg(vector.z), real_neg(vector.x));
    xz_t xz = xz_rotate(vector, me->location.angle);
    xz.z = real_neg(xz.z);
    return xz;
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

void renderer_clipByFov(const Renderer* me, lineSeg_t* wallSeg)
{
    xz_t leftIntersection, rightIntersection;
    bool_t intersectsLeft = xz_lineIntersect(*wallSeg, me->leftFovSeg, &leftIntersection);
    bool_t intersectsRight = xz_lineIntersect(*wallSeg, me->rightFovSeg, &rightIntersection);
    real_t wallPhaseLeft = intersectsLeft ? xz_linePhase(*wallSeg, leftIntersection) : real_zero;
    real_t wallPhaseRight = intersectsRight ? xz_linePhase(*wallSeg, rightIntersection) : real_zero;
    bool_t inWallSegLeft = real_inBetween(wallPhaseLeft, real_zero, real_one);
    bool_t inWallSegRight = real_inBetween(wallPhaseRight, real_zero, real_one);

    if (real_compare(wallSeg->start.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        wallSeg->start.xz = (real_compare(rightIntersection.z, real_zero) > 0 && inWallSegRight)
            ? rightIntersection
            : leftIntersection;
    }

    if (real_compare(wallSeg->end.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        wallSeg->end.xz = (real_compare(leftIntersection.z, real_zero) > 0 && inWallSegLeft)
            ? leftIntersection
            : rightIntersection;
    }
}

void renderer_project(const Renderer* me, const Sector* sector, const lineSeg_t* transformedSeg, WallSection* projected)
{
    const real_t halfHeight = real_div(real_from_int(sector->height), real_from_int(2));
    const real_t relHeightOffset = real_sub(me->location.height, real_from_int(sector->heightOffset));
#define scale_height(value) (real_mul(real_from_int(HALF_RENDERER_HEIGHT), (value)))
    const real_t scaledWallHeight =    scale_height(real_add(halfHeight, relHeightOffset));
    const real_t negScaledWallHeight = scale_height(real_add(real_neg(halfHeight), relHeightOffset));
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

void renderer_renderWall(Renderer* me, GColor* framebuffer, const Sector* sector, int wallIndex)
{
    const Wall* wall = &sector->walls[wallIndex];
    lineSeg_t wallSeg;
    renderer_transformWall(me, sector, wallIndex, &wallSeg);
    if (real_compare(wallSeg.start.xz.z, real_zero) < 0 && real_compare(wallSeg.end.xz.z, real_zero) < 0)
        return;

    renderer_clipByFov(me, &wallSeg);

    WallSection p;
    renderer_project(me, sector, &wallSeg, &p);
    if (p.left.x >= p.right.x || p.right.x < 0 || p.left.x >= RENDERER_WIDTH)
        return;

    // render wall
    for (int x = max(0, p.left.x); x <= min(RENDERER_WIDTH - 1, p.right.x); x++) {
        GColor* curPixel = framebuffer + x * RENDERER_HEIGHT;
        int yCurStart = lerpi(x, p.left.x, p.right.x, p.left.yStart, p.right.yStart);
        int yCurEnd = lerpi(x, p.left.x, p.right.x, p.left.yEnd, p.right.yEnd);
        if (yCurEnd < 0 || yCurStart >= RENDERER_HEIGHT)
            continue;

        int y;
        for (y = 0; y < max(0, yCurStart); y++)
            *(curPixel++) = sector->floorColor;
        for (; y <= min(RENDERER_HEIGHT - 1, yCurEnd); y++)
            *(curPixel++) = wall->color;
        for (; y < RENDERER_HEIGHT; y++)
            *(curPixel++) = sector->ceilColor;
    }
}

void renderer_renderSector(Renderer* renderer, GColor* framebuffer, const Sector* sector)
{
    for (int i = 0; i < sector->wallCount; i++)
        renderer_renderWall(renderer, framebuffer, sector, i);
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
    const Sector* startSector = &renderer->level->sectors[renderer->location.sector];
    renderer_renderSector(renderer, framebuffer, startSector);
};

void renderer_rotate(Renderer* renderer, int angle)
{
    renderer->location.angle = real_add(renderer->location.angle, real_degToRad(real_from_int(angle)));
}

void renderer_move(Renderer* renderer, xz_t directions)
{
    renderer_moveLocation(renderer, directions);
}

void renderer_moveHorizontal(Renderer* renderer, xy_t directions)
{
    renderer->location.height = real_add(renderer->location.height, directions.y);
}

void renderer_moveTo(Renderer* renderer, Location relativOrigin)
{
    renderer->location = relativOrigin;
}
