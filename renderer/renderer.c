#include "renderer_internal.h"
#include "algebra.h"
#include "platform.h"

#define NEAR_PLANE 1.0f
#define FAR_PLANE 500

int rendererColorFormat_getStride(RendererColorFormat format)
{
    static const int RENDERER_STRIDE[] = {
        [RendererColorFormat_8BitColor] = RENDERER_HEIGHT,
        [RendererColorFormat_1BitBW] = ((RENDERER_HEIGHT + 7) / 8 + 3) / 4 * 4
    };
    return RENDERER_STRIDE[format];
}

Renderer* renderer_init()
{
    Renderer* me = (Renderer*)malloc(sizeof(Renderer));
    if (me == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate renderer");
        return NULL;
    }
    memset(me, 0, sizeof(Renderer));

    renderer_setFieldOfView(me, real_degToRad(real_from_int(60)));
    me->eyeHeight = real_from_int(12);

    return me;
}

void renderer_free(Renderer* me)
{
    if (me == NULL)
        return;
    free(me);
}

void renderer_setFieldOfView(Renderer* me, real_t verFov)
{
    me->fov = verFov;
    const real_t horFov = real_mul(verFov, real_div(real_from_int(RENDERER_WIDTH), real_from_int(RENDERER_HEIGHT)));
    const real_t halfHorFov = real_div(horFov, real_from_int(2));
    const real_t halfVerFov = real_div(verFov, real_from_int(2));

    xz_t nearPlane, farPlane;
    nearPlane.z = real_from_float(NEAR_PLANE);
    farPlane.z = real_from_int(FAR_PLANE);

    const real_t tanHalfHorFov = real_tan(halfHorFov);
    const real_t tanHalfVerFov = real_tan(halfVerFov);
    const real_t minus_one = real_from_int(-1);
    nearPlane.x = real_mul(tanHalfHorFov, nearPlane.z);
    farPlane.x = real_mul(tanHalfHorFov, farPlane.z);
    me->leftFovSeg.start.xz = xz(real_mul(minus_one, nearPlane.x), nearPlane.z);
    me->leftFovSeg.end.xz = xz(real_mul(minus_one, farPlane.x), farPlane.z);
    me->rightFovSeg.start.xz = nearPlane;
    me->rightFovSeg.end.xz = farPlane;
    me->horFovScale = real_div(real_from_int(HALF_RENDERER_WIDTH), tanHalfHorFov);
    me->verFovScale = real_div(real_from_int(HALF_RENDERER_HEIGHT), tanHalfVerFov);
}

void renderer_setLevel(Renderer* renderer, const Level* level)
{
    renderer->level = level;
    renderer->location = level->playerStart;
    location_updateSector(&renderer->location, level);
}

void renderer_setTextureManager(Renderer* me, TextureManagerHandle handle)
{
    me->textureManager = handle;
}

xz_t renderer_transformVector(const Renderer* me, xz_t vector)
{
    return xz_rotate(vector, me->location.angle);
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
    lineSeg.end.xz = sector->walls[wallIndex].startCorner;
    lineSeg.start.xz = sector->walls[(wallIndex + 1) % sector->wallCount].startCorner;
    renderer_transformLine(me, &lineSeg, result);
}

typedef struct
{
    struct {
        int x;
        int yStart, yEnd;
    } left, right;
} WallSection;

bool_t renderer_clipByFov(const Renderer* me, lineSeg_t* wallSeg, TexCoord* texCoord)
{
    xz_t leftIntersection, rightIntersection;
    bool_t intersectsLeft = xz_lineIntersect(*wallSeg, me->leftFovSeg, &leftIntersection);
    bool_t intersectsRight = xz_lineIntersect(*wallSeg, me->rightFovSeg, &rightIntersection);
    real_t wallPhaseLeft = intersectsLeft ? xz_linePhase(*wallSeg, leftIntersection) : real_zero;
    real_t wallPhaseRight = intersectsRight ? xz_linePhase(*wallSeg, rightIntersection) : real_zero;
    bool_t inWallSegLeft = real_inBetween(wallPhaseLeft, real_zero, real_one);
    bool_t inWallSegRight = real_inBetween(wallPhaseRight, real_zero, real_one);
    real_t texCoordAmpl = real_abs(real_sub(texCoord->start.x, texCoord->end.x));
    real_t texCoordStart = real_min(texCoord->start.x, texCoord->end.x);
    bool_t result = true;

    if (real_compare(wallSeg->start.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        bool_t useLeftIntersection = (real_compare(leftIntersection.z, real_zero) > 0 && inWallSegLeft);
        wallSeg->start.xz = useLeftIntersection
           ? leftIntersection
            : inWallSegRight ? rightIntersection : (result = false, xz_zero);
        if (useLeftIntersection)
            texCoord->start.x = real_add(real_mul(wallPhaseLeft, texCoordAmpl), texCoordStart);
    }

    if (real_compare(wallSeg->end.xz.z, me->leftFovSeg.start.xz.z) <= 0)
    {
        bool_t useRightIntersection = (real_compare(rightIntersection.z, real_zero) > 0 && inWallSegRight);
        wallSeg->end.xz = useRightIntersection
            ? rightIntersection
            : inWallSegLeft ? leftIntersection : (result = false, xz_zero);
        if (useRightIntersection)
            texCoord->end.x = real_add(real_mul(wallPhaseRight, texCoordAmpl), texCoordStart);
    }
    return result;
}

int renderer_projectValue(real_t value, real_t depth, real_t fovScale, int halfSize)
{
    return real_to_int(
        real_div(real_mul(value, fovScale), depth)
    ) + halfSize;
}

void renderer_project(const Renderer* me, const Sector* sector, const lineSeg_t* transformedSeg, WallSection* projected)
{
    const real_t nominalYStart = real_sub(real_from_int(sector->heightOffset), real_add(me->location.height, me->eyeHeight));
    const real_t nominalYEnd = real_add(nominalYStart, real_from_int(sector->height));
    const xz_t startT = transformedSeg->start.xz;
    const xz_t endT = transformedSeg->end.xz;

    projected->left.x = renderer_projectValue(startT.x, startT.z, me->horFovScale, HALF_RENDERER_WIDTH);
    projected->left.yStart = renderer_projectValue(nominalYStart, startT.z, me->verFovScale, HALF_RENDERER_HEIGHT);
    projected->left.yEnd = renderer_projectValue(nominalYEnd, startT.z, me->verFovScale, HALF_RENDERER_HEIGHT);

    projected->right.x = renderer_projectValue(endT.x, endT.z, me->horFovScale, HALF_RENDERER_WIDTH);
    projected->right.yStart = renderer_projectValue(nominalYStart, endT.z, me->verFovScale, HALF_RENDERER_HEIGHT);
    projected->right.yEnd = renderer_projectValue(nominalYEnd, endT.z, me->verFovScale, HALF_RENDERER_HEIGHT);
}

void renderer_renderFilledSpan(Renderer* me, RendererTarget target, int x, int yWallLower, int yWallUpper, int yFillLower, int yFillUpper, real_t xNorm, TexCoord texCoord, const lineSeg_t* wallSeg, const Texture* texture)
{
    UNUSED(me);
    if (target.colorFormat != RendererColorFormat_8BitColor)
        return;
    GColor* const framebufferColumn =  ((GColor*)target.framebuffer) + x * RENDERER_HEIGHT;

    // Calculate texture column
    real_t invZLerped = real_lerp(xNorm, real_reciprocal(wallSeg->start.xz.z), real_reciprocal(wallSeg->end.xz.z));
    real_t invTexLerped = real_lerp(xNorm,
        real_div(texCoord.start.x, wallSeg->start.xz.z),
        real_div(texCoord.end.x, wallSeg->end.xz.z)
    );
    real_t texLerped = real_div(invTexLerped, invZLerped);
    int texCol = real_to_int(real_mul(texLerped, real_from_int(texture->size.w)));

    // Calculate texture row (start and increment)
    real_t yNormalized = real_div(real_from_int(yFillLower - yWallLower), real_from_int(yWallUpper - yWallLower));
    real_t texRow = real_mul(real_lerp(yNormalized, texCoord.start.y, texCoord.end.y), real_from_int(texture->size.h));
    real_t texRowIncr = real_div(
        real_mul(real_sub(texCoord.end.y, texCoord.start.y), real_from_int(texture->size.h)),
        real_from_int(yWallUpper - yWallLower + 1));

    // Set pixels
    GColor* curPixel = framebufferColumn + yFillLower;
    for (int y = yFillLower; y <= yFillUpper; y++) {
        int texRowI = real_to_int(texRow);
        *(curPixel++) = texture->pixels[
            (texRowI % texture->size.h) * texture->size.w +
                (texCol % texture->size.w)
        ];
        texRow = real_add(texRow, texRowIncr);
    }
}

void renderer_renderContourSpan(Renderer* me, RendererTarget target, int x, int yStart, int yEnd)
{
    UNUSED(me);
    assert(x >= 0 && x < RENDERER_WIDTH);
    if (target.colorFormat != RendererColorFormat_1BitBW)
        return;
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* const framebufferColumn = (uint8_t*)target.framebuffer + x * stride;
    const BoundarySet* drawBoundary = &me->boundarySets[me->curBoundarySet];
    yStart = max(yStart, drawBoundary->yBottom[x]);
    yEnd = min(yEnd, drawBoundary->yTop[x]);

    for (int y = yStart; y <= yEnd; y++)
    {
        uint8_t* const pixelByte = framebufferColumn + y / 8;
        *pixelByte |= 1 << (y % 8);
    }
}

void renderer_renderWall(Renderer* me, RendererTarget target, const DrawRequest* request, int wallIndex)
{
    const Sector* const sector = request->sector;
    const Wall* const wall = &sector->walls[wallIndex];

    lineSeg_t wallSeg;
    renderer_transformWall(me, sector, wallIndex, &wallSeg);
    bool isWallStartBehind = real_compare(wallSeg.start.xz.z, real_zero) < 0;
    bool isWallEndBehind = real_compare(wallSeg.end.xz.z, real_zero) < 0;
    if (isWallStartBehind && isWallEndBehind)
        return;

    TexCoord texCoord = wall->texCoord;
    if (!renderer_clipByFov(me, &wallSeg, &texCoord))
        return;

    WallSection p;
    renderer_project(me, sector, &wallSeg, &p);
    if (p.left.x >= p.right.x || p.right.x < request->left || p.left.x > request->right)
        return;

    const Sector* targetSector = &me->level->sectors[wall->portalTo];
    int portalNomStart = -1, portalNomEnd = -1;
    if (wall->portalTo >= 0 && targetSector != request->sourceSector) {
        portalNomStart = clampi(0, targetSector->heightOffset - sector->heightOffset, sector->height);
        portalNomEnd = clampi(0, portalNomStart + targetSector->height, sector->height);
        drawRequestStack_push(&me->drawRequests, targetSector,
            max(request->left, p.left.x), min(request->right, p.right.x), sector);
    }

    // render wall
    const BoundarySet* drawBoundary = &me->boundarySets[me->curBoundarySet];
    BoundarySet* nextBoundary = &me->boundarySets[!me->curBoundarySet];
    const Texture* const texture = texture_load(me->textureManager, wall->texture);
    const int renderLeft = max(request->left, p.left.x);
    const int renderRight = min(request->right, p.right.x);
    BresenhamIterator upperIt, lowerIt;
    bresenham_init(&lowerIt,
        renderLeft, lerpi(renderLeft, p.left.x, p.right.x, p.left.yStart, p.right.yStart),
        renderRight, lerpi(renderRight, p.left.x, p.right.x, p.left.yStart, p.right.yStart));
    bresenham_init(&upperIt,
        renderLeft, lerpi(renderLeft, p.left.x, p.right.x, p.left.yEnd, p.right.yEnd),
        renderRight, lerpi(renderRight, p.left.x, p.right.x, p.left.yEnd, p.right.yEnd));

    // render wall side contours
    if (renderLeft == p.left.x && !isWallStartBehind)
        renderer_renderContourSpan(me, target, renderLeft, lowerIt.y0, upperIt.y0);
    if (renderRight == p.right.x && !isWallEndBehind)
        renderer_renderContourSpan(me, target, renderRight, lowerIt.y1, upperIt.y1);

    // render wall spans
    BresenhamStep upperStep, lowerStep;
    int x = renderLeft;
    do {
        const int yBottom = drawBoundary->yBottom[x];
        const int yTop = drawBoundary->yTop[x];

        do {
            upperStep = bresenham_step(&upperIt);
            renderer_renderContourSpan(me, target, x, upperIt.y0, upperIt.y0);
        } while (upperStep != BRESENHAMSTEP_X && upperStep != BRESENHAMSTEP_NONE);
        do {
            lowerStep = bresenham_step(&lowerIt);
            renderer_renderContourSpan(me, target, x, lowerIt.y0, lowerIt.y0);
        } while (lowerStep != BRESENHAMSTEP_X && lowerStep != BRESENHAMSTEP_NONE);
        assert(upperIt.x0 == lowerIt.x0);

        int yPortalStart = upperIt.y0, yPortalEnd = yTop + 1;
        if (wall->portalTo >= 0) {
            nextBoundary->yBottom[x] = yPortalStart = clampi(yBottom, lerpi(portalNomStart, 0, sector->height, lowerIt.y0, upperIt.y0), yTop);
            yPortalEnd = lerpi(portalNomEnd, 0, sector->height, lowerIt.y0, upperIt.y0);
            nextBoundary->yTop[x] = clampi(yBottom, yPortalEnd, yTop);

            renderer_renderContourSpan(me, target, x, yPortalStart, yPortalStart);
            renderer_renderContourSpan(me, target, x, yPortalEnd, yPortalEnd);
        }

        me->wallBoundaries.yBottom[x] = max(yBottom, lowerIt.y0);
        me->wallBoundaries.yTop[x] = min(yTop, upperIt.y0 - 1);

        real_t xNorm = real_div(real_from_int(x - p.left.x), real_from_int(p.right.x - p.left.x));
        renderer_renderFilledSpan(me, target, x,
            lowerIt.y0, upperIt.y0, max(yBottom, lowerIt.y0), min(yTop, yPortalStart - 1),
            xNorm, texCoord, &wallSeg, texture);

        if (wall->portalTo >= 0) {
            renderer_renderFilledSpan(me, target, x,
                lowerIt.y0, upperIt.y0, max(yBottom, yPortalEnd), min(yTop, upperIt.y0),
                xNorm, texCoord, &wallSeg, texture);
        }

        x++;
    } while(upperStep != BRESENHAMSTEP_NONE);
    assert(lowerStep == BRESENHAMSTEP_NONE);

    texture_free(me->textureManager, texture);
}

void renderer_renderSlabColumns(Renderer* renderer, RendererTarget target, const DrawRequest* request, const short* bottomSet, const short* topSet, bool isCeil)
{
    if (target.colorFormat != RendererColorFormat_8BitColor)
        return;
    GColor* const framebuffer = (GColor*)target.framebuffer;

    const int minX = request->left, maxX = request->right;
    const GColor color = isCeil
        ? request->sector->ceilColor
        : request->sector->floorColor;
    int top1, top2, bottom1, bottom2;
    top1 = -10000;
    bottom1 = 10000;

    for (int x = minX; x <= maxX + 1; x++)
    {
        top2 = x <= maxX ? topSet[x] : -10000;
        bottom2 = x <= maxX ? bottomSet[x] : 10000;

        while (top1 < top2 && top2 >= bottom2)
        {
            renderer->spanStart[top2] = x;
            top2--;
        }
        while (bottom1 > bottom2 && bottom2 <= top2)
        {
            renderer->spanStart[bottom2] = x;
            bottom2++;
        }
        while (top1 > top2 && top1 >= bottom1)
        {
            assert(renderer->spanStart[top1] >= 0 && renderer->spanStart[top1] < RENDERER_WIDTH);
            assert(top1 >= 0 && top1 < RENDERER_HEIGHT);
            for (int i = renderer->spanStart[top1]; i < x; i++)
                framebuffer[i * RENDERER_HEIGHT + top1] = color;
            top1--;
        }
        while (bottom1 < bottom2 && bottom1 <= top1)
        {
            assert(renderer->spanStart[bottom1] >= 0 && renderer->spanStart[bottom1] < RENDERER_WIDTH);
            assert(bottom1 >= 0 && bottom1 < RENDERER_HEIGHT);
            for (int i = renderer->spanStart[bottom1]; i < x; i++)
                framebuffer[i * RENDERER_HEIGHT + bottom1] = color;
            bottom1++;
        }

        top1 = topSet[x];
        bottom1 = bottomSet[x];
    }
}

void renderer_renderSlabs(Renderer* me, RendererTarget target, const DrawRequest* request)
{
    renderer_renderSlabColumns(me, target, request,
        me->wallBoundaries.yTop,
        me->boundarySets[me->curBoundarySet].yTop,
        true);
    renderer_renderSlabColumns(me, target, request,
        me->boundarySets[me->curBoundarySet].yBottom,
        me->wallBoundaries.yBottom,
        false);
}

void renderer_renderSector(Renderer* renderer, RendererTarget target, const DrawRequest* request)
{
    for (int y = 0; y < RENDERER_HEIGHT; y++)
        renderer->spanStart[y] = -1;
    for (int i = 0; i < request->sector->wallCount; i++)
        renderer_renderWall(renderer, target, request, i);
    renderer_renderSlabs(renderer, target, request);
}

void renderer_render(Renderer* renderer, RendererTarget target)
{
    memset(target.framebuffer, 0, RENDERER_WIDTH * rendererColorFormat_getStride(target.colorFormat));

    if (renderer->level == NULL || renderer->location.sector < 0)
        return;
    memset(renderer->boundarySets[0].yBottom, 0, sizeof(renderer->boundarySets[0].yBottom));
    memset(renderer->boundarySets[1].yBottom, 0, sizeof(renderer->boundarySets[1].yBottom));
    memset(renderer->wallBoundaries.yBottom, 0, sizeof(renderer->wallBoundaries.yBottom));
    for (int i = 0; i < RENDERER_WIDTH; i++) {
        renderer->wallBoundaries.yTop[i] =
        renderer->boundarySets[0].yTop[i] =
            renderer->boundarySets[1].yTop[i] = RENDERER_HEIGHT - 1;
    }
    renderer->curBoundarySet = 0;

    drawRequestStack_reset(&renderer->drawRequests);
    drawRequestStack_push(&renderer->drawRequests,
        &renderer->level->sectors[renderer->location.sector],
        0, RENDERER_WIDTH - 1, NULL);
    drawRequestStack_nextDepth(&renderer->drawRequests);

    const DrawRequest* curRequest = drawRequestStack_pop(&renderer->drawRequests);
    int lastDepth = 0;
    while (curRequest != NULL)
    {
        if (lastDepth != curRequest->depth)
        {
            lastDepth = curRequest->depth;
            renderer->curBoundarySet = !renderer->curBoundarySet;
            drawRequestStack_nextDepth(&renderer->drawRequests);
        }
        renderer_renderSector(renderer, target, curRequest);
        curRequest = drawRequestStack_pop(&renderer->drawRequests);
    }
};

void renderer_rotate(Renderer* renderer, real_t angle)
{
    renderer->location.angle = real_add(renderer->location.angle, real_degToRad(angle));
}

void renderer_move(Renderer* renderer, xz_t directions)
{
    directions = xz_rotate(directions, real_neg(renderer->location.angle)); // angle needs the be negated because xz_rotate is considering a righthand rotation to be in the positiv and left negativ (world space)
    renderer->location.position = xz_add(renderer->location.position, directions);
    location_updateSector(&renderer->location, renderer->level);
}

void renderer_moveVertical(Renderer* renderer, xy_t directions)
{
    renderer->location.height = real_add(renderer->location.height, directions.y);
}

void renderer_moveTo(Renderer* renderer, Location relativOrigin)
{
    renderer->location = relativOrigin;
}

void drawRequestStack_reset(DrawRequestStack* stack)
{
    stack->start = stack->end = stack->depth = 0;
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
        .depth = stack->depth,
        .sourceSector = sourceSector
    };
}

void drawRequestStack_nextDepth(DrawRequestStack* stack)
{
    stack->depth++;
}

const DrawRequest* drawRequestStack_pop(DrawRequestStack* stack)
{
    if (stack->start == stack->end)
        return NULL;
    const DrawRequest* result = &stack->requests[stack->start];
    stack->start = (stack->start + 1) % MAX_DRAW_DEPTH;
    return result;
}

void bresenham_init(BresenhamIterator* it, int x0, int y0, int x1, int y1)
{
    it->x0 = x0;
    it->y0 = y0;
    it->x1 = x1;
    it->y1 = y1;
    it->dx = abs(x1 - x0);
    it->dy = -abs(y1 - y0);
    it->sx = x0 < x1 ? 1 : -1;
    it->sy = y0 < y1 ? 1 : -1;
    it->err = it->dx + it->dy;
    it->isFirstStep = true;
}

BresenhamStep bresenham_step(BresenhamIterator* it)
{
    if (it->isFirstStep) {
        it->isFirstStep = false;
        return BRESENHAMSTEP_INIT;
    }
    if (it->x0 == it->x1 && it->y0 == it->y1)
        return BRESENHAMSTEP_NONE;
    if (it->err * 2 > it->dy) {
        it->err += it->dy;
        it->x0 += it->sx;
        return BRESENHAMSTEP_X;
    }
    if (it->err * 2 < it->dx) {
        it->err += it->dx;
        it->y0 += it->sy;
        return BRESENHAMSTEP_Y;
    }
    assert(false && "This should never have happened");
    return BRESENHAMSTEP_NONE;
}
