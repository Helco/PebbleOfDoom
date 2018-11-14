#include "renderer_internal.h"
#include "algebra.h"

Renderer* renderer_init()
{
    Renderer* this = (Renderer*)malloc(sizeof(Renderer));
    if (this == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate renderer");
        return NULL;
    }

    this->wall.end = xz(real_from_int(0), real_from_int(50));
    this->wall.start = xz(real_from_int(70), real_from_int(50));
    this->wall.height = real_from_int(25);
    this->wall.heightOffset = real_zero;
    this->wall.floorColor = GColorFromRGB(0, 0, 255);
    this->wall.wallColor = GColorFromRGB(0, 255, 0);
    this->wall.ceilColor = GColorFromRGB(255, 0, 0);

    this->wall2.start = xz(real_from_int(0), real_from_int(50));
    this->wall2.end = xz(real_from_int(0), real_from_int(-30));
    this->wall2.height = real_from_int(25);
    this->wall2.heightOffset = real_zero;
    this->wall2.floorColor = GColorFromRGB(0, 0, 255);
    this->wall2.wallColor = GColorFromRGB(255, 0, 255);
    this->wall2.ceilColor = GColorFromRGB(255, 0, 0);

    this->wall3.start = xz(real_from_int(0), real_from_int(-30));
    this->wall3.end = xz(real_from_int(70), real_from_int(50));
    this->wall3.height = real_from_int(25);
    this->wall3.heightOffset = real_zero;
    this->wall3.floorColor = GColorFromRGB(0, 0, 255);
    this->wall3.wallColor = GColorFromRGB(0, 255, 255);
    this->wall3.ceilColor = GColorFromRGB(255, 0, 0);

    this->playerLocation.position = xz(real_from_int(20), real_from_int(20));
    this->playerLocation.angle = real_degToRad(real_from_int(0));
    this->playerLocation.height = real_zero;

    this->halfFov = real_degToRad(real_from_int(30));

    xz_t nearPlane, farPlane;
    nearPlane.z = real_from_float(1.0f);
    farPlane.z = real_from_int(500);

    const real_t tanHalfFov = real_tan(this->halfFov);
    const real_t minus_one = real_from_int(-1);
    nearPlane.x = real_mul(tanHalfFov, nearPlane.z);
    farPlane.x = real_mul(tanHalfFov, farPlane.z);
    this->leftFovSeg.start.xz = xz(real_mul(minus_one, nearPlane.x), nearPlane.z);
    this->leftFovSeg.end.xz = xz(real_mul(minus_one, farPlane.x), farPlane.z);
    this->rightFovSeg.start.xz = nearPlane;
    this->rightFovSeg.end.xz = farPlane;
    this->fovStuff = real_div(real_from_int(-HALF_RENDERER_WIDTH), tanHalfFov);

    return this;
}

void renderer_free(Renderer* this)
{
    if (this == NULL)
        return;
    free(this);
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
    return myxz_rotate(vector, me->playerLocation.angle);
}

xz_t renderer_transformPoint(const Renderer* me, xz_t point)
{
    return renderer_transformVector(me, xz_sub(point, me->playerLocation.position));
}

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result)
{
    result->start.xz = renderer_transformPoint(me, line->start.xz);
    result->end.xz = renderer_transformPoint(me, line->end.xz);
}

void renderer_transformWall(const Renderer* me, const Wall* wall, lineSeg_t* result)
{
    lineSeg_t lineSeg;
    lineSeg.start.xz = wall->start;
    lineSeg.end.xz = wall->end;
    renderer_transformLine(me, &lineSeg, result);
}

typedef struct
{
    struct {
        int x;
        int yStart, yEnd;
    } left, right;
} WallSection;

void renderer_project(const Renderer* me, const Wall* wall, const lineSeg_t* transformedSeg, WallSection* projected)
{
    const real_t halfHeight = real_div(wall->height, real_from_int(2));
    const real_t relHeightOffset = me->playerLocation.height - wall->heightOffset;
#define scale_height(value) (real_mul(real_from_int(HALF_RENDERER_HEIGHT), (value)))
    const real_t scaledWallHeight =    scale_height(real_add(halfHeight, relHeightOffset));
    const real_t negScaledWallHeight = scale_height(real_add(real_neg(halfHeight), relHeightOffset));
#undef scale_height
    const xz_t startT = transformedSeg->start.xz;
    const xz_t endT = transformedSeg->end.xz;

#define div_and_int(value, z) (real_to_int(real_div((value), (z))))
    projected->left.x =       div_and_int(real_mul(startT.x, me->fovStuff), startT.z) + HALF_RENDERER_WIDTH;
    projected->left.yStart =  div_and_int(negScaledWallHeight, startT.z)              + HALF_RENDERER_HEIGHT;
    projected->left.yEnd =    div_and_int(scaledWallHeight, startT.z)                 + HALF_RENDERER_HEIGHT;

    projected->right.x =      div_and_int(real_mul(endT.x, me->fovStuff), endT.z)     + HALF_RENDERER_WIDTH;
    projected->right.yStart = div_and_int(negScaledWallHeight, endT.z)                + HALF_RENDERER_HEIGHT;
    projected->right.yEnd =   div_and_int(scaledWallHeight, endT.z)                   + HALF_RENDERER_HEIGHT;
#undef div_and_int
}

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

void renderer_renderWall(Renderer* this, GColor* framebuffer, const Wall* wall)
{
    lineSeg_t wallSeg;
    renderer_transformWall(this, wall, &wallSeg);
    if (wallSeg.start.xz.z < 0 && wallSeg.end.xz.z < 0)
        return;

    renderer_clipByFov(this, &wallSeg);

    WallSection p;
    renderer_project(this, wall, &wallSeg, &p);
    if (p.left.x >= p.right.x || p.right.x < 0 || p.left.x >= RENDERER_WIDTH)
        return;

    // render wall
    for (int x = max(0, p.left.x); x <= min(RENDERER_WIDTH - 1, p.right.x); x++) {
        GColor* curPixel = framebuffer + x * RENDERER_HEIGHT;
        int yCurStart = (x - p.left.x) * (p.right.yStart - p.left.yStart) / (p.right.x - p.left.x) + p.left.yStart;
        int yCurEnd = (x - p.left.x) * (p.right.yEnd - p.left.yEnd) / (p.right.x - p.left.x) + p.left.yEnd;

        int y;
        for (y = 0; y < max(0, yCurStart); y++)
            *(curPixel++) = wall->floorColor;
        for (; y <= min(RENDERER_HEIGHT - 1, yCurEnd); y++)
            *(curPixel++) = wall->wallColor;
        for (; y < RENDERER_HEIGHT; y++)
            *(curPixel++) = wall->ceilColor;
    }
}

void renderer_movePlayerInPlayerSpace(Renderer* renderer, xz_t xz)
{
    xz = xz_rotate(xz, renderer->playerLocation.angle);
    renderer->playerLocation.position = xz_add(renderer->playerLocation.position, xz);
}

void renderer_render(Renderer* renderer, GColor* framebuffer)
{
    memset(framebuffer, 0, RENDERER_WIDTH * RENDERER_HEIGHT);
    renderer_renderWall(renderer, framebuffer, &renderer->wall);
    renderer_renderWall(renderer, framebuffer, &renderer->wall2);
    renderer_renderWall(renderer, framebuffer, &renderer->wall3);
};

void renderer_renderRotateRight(Renderer* renderer, GColor* framebuffer)
{
    renderer->playerLocation.angle = real_add(renderer->playerLocation.angle, real_degToRad(1));
    renderer_render(renderer, framebuffer);
}

void renderer_renderRotateLeft(Renderer* renderer, GColor* framebuffer)
{
    renderer->playerLocation.angle = real_sub(renderer->playerLocation.angle, real_degToRad(1));
    renderer_render(renderer, framebuffer);
}

void renderer_renderPlayerForward(Renderer* renderer, GColor* framebuffer)
{
    renderer_movePlayerInPlayerSpace(renderer, xz(real_one, real_zero));
    renderer_render(renderer, framebuffer);
}

void renderer_renderPlayerBackwards(Renderer* renderer, GColor* framebuffer)
{
    renderer_movePlayerInPlayerSpace(renderer, xz(-real_one, real_zero));
    renderer_render(renderer, framebuffer);
}

void renderer_renderPlayerRight(Renderer* renderer, GColor* framebuffer)
{
    renderer_movePlayerInPlayerSpace(renderer, xz(real_zero, real_one));
    renderer_render(renderer, framebuffer);
}

void renderer_renderPlayerLeft(Renderer* renderer, GColor* framebuffer)
{
    renderer_movePlayerInPlayerSpace(renderer, xz(real_zero, -real_one));
    renderer_render(renderer, framebuffer);
}

void renderer_renderNewPlayerLocation(Renderer* renderer, GColor* framebuffer, PlayerLocation playerLocation)
{
    renderer->playerLocation = playerLocation;
    renderer_render(renderer, framebuffer);
}
