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
    this->wall.floorColor.argb = 0b00001111; // blue
    this->wall.wallColor.argb = 0b00110011; // green
    this->wall.ceilColor.argb = 0b11000011; // red

    this->wall2.start = xz(real_from_int(0), real_from_int(50));
    this->wall2.end = xz(real_from_int(0), real_from_int(-30));
    this->wall2.height = real_from_int(25);
    this->wall2.floorColor.argb = 0b00001111; // blue
    this->wall2.wallColor.argb = 0b11001111; // magic pink
    this->wall2.ceilColor.argb = 0b11000011; // red

    this->wall3.start = xz(real_from_int(0), real_from_int(-30));
    this->wall3.end = xz(real_from_int(70), real_from_int(50));
    this->wall3.height = real_from_int(25);
    this->wall3.floorColor.argb = 0b00001111; // blue
    this->wall3.wallColor.argb = 0b11110011; // magic something
    this->wall3.ceilColor.argb = 0b11000011; // red

    this->pos = xz(real_from_int(68), real_from_int(48));
    this->angle = real_degToRad(real_from_int(223));
    this->halfFov = real_degToRad(real_from_int(60));
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
    this->fovStuff = real_div(real_from_int(-RENDERER_WIDTH / 2), tanHalfFov);

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

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(a) ((a)<0?-(a):(a))

xz_t renderer_transformVector(const Renderer* me, xz_t vector)
{
    return myxz_rotate(vector, me->angle);
}

xz_t renderer_transformPoint(const Renderer* me, xz_t point)
{
    return renderer_transformVector(me, xz_sub(point, me->pos));
}

void renderer_transformLine(const Renderer* me, const lineSeg_t* line, lineSeg_t* result)
{
    result->start.xz = renderer_transformPoint(me, line->start.xz);
    result->end.xz = renderer_transformPoint(me, line->end.xz);
    /*if (real_compare(result->start.xz.x, result->end.xz.x) < 0)
    {
        xz_t tmp = result->start.xz;
        result->start.xz = result->end.xz;
        result->end.xz = tmp;
    }*/
}

void renderer_transformWall(const Renderer* me, const Wall* wall, lineSeg_t* result)
{
    lineSeg_t lineSeg;
    lineSeg.start.xz = wall->start;
    lineSeg.end.xz = wall->end;
    renderer_transformLine(me, &lineSeg, result);
}

void renderer_renderWall(Renderer* this, GColor* framebuffer, const Wall* wall)
{
    // Transform corners
    lineSeg_t wallSeg;
    renderer_transformWall(this, wall, &wallSeg);
    xz_t startT = wallSeg.start.xz;
    xz_t endT = wallSeg.end.xz;
    if (startT.z < 0 && endT.z < 0)
        return;

    // Intersect with FOV rays
    xz_t leftIntersection, rightIntersection;
    bool_t intersectsLeft = xz_lineIntersect(wallSeg, this->leftFovSeg, &leftIntersection);
    bool_t intersectsRight = xz_lineIntersect(wallSeg, this->rightFovSeg, &rightIntersection);
    real_t phaseLeft = intersectsLeft ? xz_linePhase(this->leftFovSeg, leftIntersection) : real_zero; // pi is > 1
    real_t phaseRight = intersectsRight ? xz_linePhase(this->rightFovSeg, rightIntersection) : real_zero;
    real_t wallPhaseLeft = intersectsLeft ? xz_linePhase(wallSeg, leftIntersection) : real_zero; // pi is > 1
    real_t wallPhaseRight = intersectsRight ? xz_linePhase(wallSeg, rightIntersection) : real_zero;
    bool_t inFovSegLeft = real_inBetween(phaseLeft, real_zero, real_one);
    bool_t inFovSegRight = real_inBetween(phaseRight, real_zero, real_one);
    bool_t inWallSegLeft = real_inBetween(wallPhaseLeft, real_zero, real_one);
    bool_t inWallSegRight = real_inBetween(wallPhaseRight, real_zero, real_one);
    bool_t should = inFovSegLeft && inFovSegRight && abs(real_signInt(wallPhaseLeft) + real_signInt(wallPhaseRight)) == 0;
    xz_t leftOffset = xz_sub(this->leftFovSeg.end.xz, this->leftFovSeg.start.xz);
    xz_t rightOffset = xz_sub(this->rightFovSeg.end.xz, this->rightFovSeg.start.xz);
    xz_t leftNormal = xz_orthogonal(leftOffset);
    xz_t rightNormal = xz_orthogonal(rightOffset);
    bool_t isStartIn =
        real_signInt(real_sub(xz_dot(leftNormal, startT), xz_dot(leftNormal, leftOffset))) < 0 &&
        real_signInt(real_sub(xz_dot(rightNormal, startT), xz_dot(rightNormal, rightOffset))) > 0;
    bool_t isEndIn =
        real_signInt(real_sub(xz_dot(leftNormal, endT), xz_dot(leftNormal, leftOffset))) < 0 &&
        real_signInt(real_sub(xz_dot(rightNormal, endT), xz_dot(rightNormal, rightOffset))) > 0;
    //should = should || (inFovSegLeft && inWallSegLeft) || (inFovSegRight && inWallSegRight);
    should = (inFovSegLeft && inWallSegLeft) || (inFovSegRight && inWallSegRight) || (isStartIn && isEndIn);
    //if (!should)
        //return;
    if (real_compare(startT.z, this->leftFovSeg.start.xz.z) <= 0)
    {
        startT = (real_compare(rightIntersection.z, real_zero) > 0 && inWallSegRight)
            ? rightIntersection
            : leftIntersection;
    }
    if (real_compare(endT.z, this->leftFovSeg.start.xz.z) <= 0)
    {
        endT = (real_compare(leftIntersection.z, real_zero) > 0 && inWallSegLeft)
            ? leftIntersection
            : rightIntersection;
    }
    
    //if ((!inFovSegLeft && !inFovSegRight))
        //return;
    //else if (inFovSegLeft)
    //    startT = leftIntersection;
    //else if (inFovSegRight)
    //    endT = rightIntersection;

    // calculate screen position
    real_t halfHeight = real_div(wall->height, real_from_int(2));
    real_t renderHeight = real_from_int(RENDERER_HEIGHT / 2);
    real_t scaledWallHeight = real_mul(renderHeight, halfHeight);
    int xLeft = real_to_int(real_div(real_mul(startT.x, this->fovStuff), startT.z)) + RENDERER_WIDTH/2;
    int xRight = real_to_int(real_div(real_mul(endT.x, this->fovStuff), endT.z)) + RENDERER_WIDTH/2;
    if (xLeft >= xRight || xRight < 0 || xLeft >= RENDERER_WIDTH)
        return;
    int yLeftStart = real_to_int(real_div(real_sub(real_zero, scaledWallHeight), startT.z)) + RENDERER_HEIGHT/2;
    int yLeftEnd = real_to_int(real_div(scaledWallHeight, startT.z)) + RENDERER_HEIGHT/2;
    int yRightStart = real_to_int(real_div(real_sub(real_zero, scaledWallHeight), endT.z)) + RENDERER_HEIGHT/2;
    int yRightEnd = real_to_int(real_div(scaledWallHeight, endT.z)) + RENDERER_HEIGHT/2;

    // render wall
    for (int x = max(0, xLeft); x <= min(RENDERER_WIDTH - 1, xRight); x++) {
        GColor* curPixel = framebuffer + x * RENDERER_HEIGHT;
        int yCurStart = (x - xLeft) * (yRightStart - yLeftStart) / (xRight - xLeft) + yLeftStart;
        int yCurEnd = (x - xLeft) * (yRightEnd - yLeftEnd) / (xRight - xLeft) + yLeftEnd;

        int y;
        for (y = 0; y < max(0, yCurStart); y++)
            *(curPixel++) = wall->floorColor;
        for (; y <= min(RENDERER_HEIGHT - 1, yCurEnd); y++)
            *(curPixel++) = wall->wallColor;
        for (; y < RENDERER_HEIGHT; y++)
            *(curPixel++) = wall->ceilColor;
    }
}

void renderer_render(Renderer* renderer, GColor* framebuffer)
{
    renderer->angle = real_add(renderer->angle, real_degToRad(1));

    memset(framebuffer, 0, RENDERER_WIDTH * RENDERER_HEIGHT);
    renderer_renderWall(renderer, framebuffer, &renderer->wall);
    renderer_renderWall(renderer, framebuffer, &renderer->wall2);
    renderer_renderWall(renderer, framebuffer, &renderer->wall3);
}
