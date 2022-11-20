#include "renderer_internal.h"
#include "algebra.h"
#include "platform.h"

#define NEAR_PLANE 1.0f
#define FAR_PLANE 500

int rendererColorFormat_getStride(RendererColorFormat format)
{
    static const int RENDERER_STRIDE[] = {
        [RendererColorFormat_8BitColor] = SCREEN_HEIGHT,
        [RendererColorFormat_1BitBW] = ((SCREEN_HEIGHT + 7) / 8 + 3) / 4 * 4
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
    if (me->transformedVertices != NULL)
        free(me->transformedVertices);
    if (me->transformedStatus != NULL)
        free(me->transformedStatus);
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

void renderer_setLevel(Renderer* me, const Level* level)
{
    int statusCount = (level->vertexCount + 31) / 32;
    me->transformedVertices = (xz_t*)realloc(me->transformedVertices, sizeof(xz_t) * level->vertexCount);
    me->transformedStatus = (uint32_t*)realloc(me->transformedStatus, sizeof(uint32_t) * statusCount);
    assert(me->transformedVertices != NULL && me->transformedStatus != NULL);

    me->level = level;
    me->location = level->playerStart;
    location_updateSector(&me->location, level);
}

void renderer_setTextureManager(Renderer* me, TextureManagerHandle handle)
{
    me->textureManager = handle;
}

TextureManagerHandle renderer_getTextureManager(Renderer* me)
{
    return me->textureManager;
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

xz_t renderer_transformVertex(const Renderer* me, int vertexI)
{
    uint32_t statusElement = vertexI / 32, statusBit = 1 << (vertexI % 32);
    if ((me->transformedStatus[statusElement] & statusBit) == 0) {
        me->transformedStatus[statusElement] |= statusBit;
        me->transformedVertices[vertexI] = renderer_transformPoint(me, me->level->vertices[vertexI]);
    }
    return me->transformedVertices[vertexI];
}

lineSeg_t renderer_transformWall(const Renderer* me, const Sector* sector, int wallIndex)
{
    return (lineSeg_t) {
        .end = { .xz = renderer_transformVertex(me, sector->walls[wallIndex].startCorner) },
        .start = { .xz = renderer_transformVertex(me, sector->walls[(wallIndex + 1) % sector->wallCount].startCorner) }
    };
}

lineSeg_t renderer_transformEntity(const Renderer* me, xz_t position, real_t radius)
{
    position = renderer_transformPoint(me, position);
    lineSeg_t l = { .start = {.xz = position}, .end = {.xz = position } };
    l.start.xz.x = real_sub(l.start.xz.x, radius);
    l.end.xz.x = real_add(l.end.xz.x, radius);
    return l;

    /* alternative billboarding was tried, but not only does this need normalize
       it also produces sloped edges so we cannot optimize for bitmap blits.

    xz_t left = xz_normalize(xz_orthogonal(position));
    left = xz_scale(left, radius);
    return (lineSeg_t) {
        .start = { .xz = xz_add(position, left) },
        .end = {.xz = xz_sub(position, left) }
    };*/
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
    int compareStartZ = real_compare(wallSeg->start.xz.z, me->leftFovSeg.start.xz.z);
    int compareEndZ = real_compare(wallSeg->end.xz.z, me->leftFovSeg.start.xz.z);
    if (compareStartZ > 0 && compareEndZ > 0)
        return true;

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

    if (compareStartZ <= 0)
    {
        bool_t useLeftIntersection = (real_compare(leftIntersection.z, real_zero) > 0 && inWallSegLeft);
        wallSeg->start.xz = useLeftIntersection
           ? leftIntersection
            : inWallSegRight ? rightIntersection : (result = false, xz_zero);
        if (useLeftIntersection)
            texCoord->start.x = real_add(real_mul(wallPhaseLeft, texCoordAmpl), texCoordStart);
    }

    if (compareEndZ <= 0)
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

int renderer_projectValue(real_t value, real_t invDepth, real_t fovScale, int halfSize)
{
    return real_to_int(
        real_mul(real_mul(value, fovScale), invDepth)
    ) + halfSize;
}

void renderer_project(const Renderer* me, real_t heightOffset, real_t height, const lineSeg_t* transformedSeg, WallSection* projected)
{
    const real_t nominalYStart = real_sub(heightOffset, real_add(me->location.height, me->eyeHeight));
    const real_t nominalYEnd = real_add(nominalYStart, height);
    const xz_t startT = transformedSeg->start.xz;
    const xz_t endT = transformedSeg->end.xz;
    const real_t invStartZ = real_reciprocal(startT.z);
    const real_t invEndZ = real_reciprocal(endT.z);

    projected->left.x = renderer_projectValue(startT.x, invStartZ, me->horFovScale, HALF_RENDERER_WIDTH);
    projected->left.yStart = renderer_projectValue(nominalYStart, invStartZ, me->verFovScale, HALF_RENDERER_HEIGHT);
    projected->left.yEnd = renderer_projectValue(nominalYEnd, invStartZ, me->verFovScale, HALF_RENDERER_HEIGHT);

    projected->right.x = renderer_projectValue(endT.x, invEndZ, me->horFovScale, HALF_RENDERER_WIDTH);
    projected->right.yStart = renderer_projectValue(nominalYStart, invEndZ, me->verFovScale, HALF_RENDERER_HEIGHT);
    projected->right.yEnd = renderer_projectValue(nominalYEnd, invEndZ, me->verFovScale, HALF_RENDERER_HEIGHT);
}

typedef void (*FilledSpanRenderFunc)(Renderer*, RendererTarget, int, int, int, int, int, real_t, TexCoord, const lineSeg_t*, const void*);

void renderer_renderFilledSpan_textured(Renderer* me, RendererTarget target, int x, int yWallLower, int yWallUpper, int yFillLower, int yFillUpper, real_t xNorm, TexCoord texCoord, const lineSeg_t* wallSeg, const void* userdata)
{
    UNUSED(me);
    if (target.colorFormat != RendererColorFormat_8BitColor)
        return;
    const Texture* texture = (const Texture*)userdata;
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

    int shift = 16;
    int texRowII = real_to_int(real_mul(texRow, real_from_int(1 << shift)));
    int texRowIncrI = real_to_int(real_mul(texRowIncr, real_from_int(1 << shift)));

    // Set pixels
    GColor* curPixel = framebufferColumn + yFillLower;
    for (int y = yFillLower; y <= yFillUpper; y++) {
        int texRowI = texRowII >> shift;
        *(curPixel++) = texture->pixels[
            (texture->size.h - 1 - (texRowI % texture->size.h)) * texture->size.w +
                (texCol % texture->size.w)
        ];
        texRowII += texRowIncrI;
    }
}

void renderer_renderFilledSpan_colored(Renderer* me, RendererTarget target, int x, int yWallLower, int yWallUpper, int yFillLower, int yFillUpper, real_t xNorm, TexCoord texCoord, const lineSeg_t* wallSeg, const void* userdata)
{
    UNUSED(me, yWallLower, yWallUpper, xNorm, texCoord, wallSeg);
    if (target.colorFormat != RendererColorFormat_8BitColor || yFillLower > yFillUpper)
        return;
    const GColor wallColor = *(GColor*)userdata;
    GColor* const framebufferColumn = ((GColor*)target.framebuffer) + x * RENDERER_HEIGHT;
    memset(framebufferColumn + yFillLower, wallColor.argb, yFillUpper - yFillLower + 1);
}

void renderer_renderContourSpan(Renderer* me, RendererTarget target, const BoundarySet* drawBoundary, int x, int yStart, int yEnd)
{
    UNUSED(me);
    assert(x >= 0 && x < RENDERER_WIDTH);
    yStart = max(yStart, drawBoundary->yBottom[x]);
    yEnd = min(yEnd, drawBoundary->yTop[x]);
    if (target.colorFormat != RendererColorFormat_1BitBW || yStart > yEnd)
        return;

    render_setBitColumn(target, x, yStart, yEnd);
}

void render_setBitColumn(RendererTarget target, int x, int yStart, int yEnd)
{
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* const framebufferColumn = (uint8_t*)target.framebuffer + x * stride;
    uint8_t* pixelByte = framebufferColumn + yStart / 8;
    int bitCount = yEnd - yStart + 1;

    { // just to group, first try to align
        *(pixelByte++) |= ((1 << min(8, bitCount)) - 1) << (yStart % 8);
        bitCount -= min(8 - (yStart % 8), bitCount);
    }

    if (bitCount >= 8) {
        int byteCount = bitCount / 8;
        memset(pixelByte, 255, byteCount);
        pixelByte += byteCount;
        bitCount = bitCount % 8;
    }

    if (bitCount > 0) {
        *pixelByte |= (1 << bitCount) - 1;
    }
}

void render_clearBitColumn(RendererTarget target, int x, int yStart, int yEnd)
{
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* const framebufferColumn = (uint8_t*)target.framebuffer + x * stride;
    uint8_t* pixelByte = framebufferColumn + yStart / 8;
    int bitCount = yEnd - yStart + 1;

    { // just to group, first try to align
        *(pixelByte++) &= ~(((1 << min(8, bitCount)) - 1) << (yStart % 8));
        bitCount -= min(8 - (yStart % 8), bitCount);
    }

    if (bitCount >= 8) {
        int byteCount = bitCount / 8;
        memset(pixelByte, 0, byteCount);
        pixelByte += byteCount;
        bitCount = bitCount % 8;
    }

    if (bitCount > 0) {
        *pixelByte &= ~((1 << bitCount) - 1);
    }
}

void render_flipBitColumn(RendererTarget target, int x, int yStart, int yEnd)
{
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* const framebufferColumn = (uint8_t*)target.framebuffer + x * stride;
    uint8_t* pixelByte = framebufferColumn + yStart / 8;
    int bitCount = yEnd - yStart + 1;

    { // just to group, first try to align
        *(pixelByte++) ^= ((1 << min(8, bitCount)) - 1) << (yStart % 8);
        bitCount -= min(8 - (yStart % 8), bitCount);
    }

    if (bitCount >= 8) {
        int byteCount = bitCount / 8;
        for (int i = 0; i < byteCount; i++)
            pixelByte[i] = ~pixelByte[i];
        pixelByte += byteCount;
        bitCount = bitCount % 8;
    }

    if (bitCount > 0) {
        *pixelByte ^= (1 << bitCount) - 1;
    }
}

void renderer_renderNonPortalContour(Renderer* me, RendererTarget target, const BoundarySet* drawBoundary,
    int x,
    int portalNomLow, int portalNomHigh, int sectorHeight,
    int drawLow, int drawHigh)
{
    const int portalDrawLow = lerpi(portalNomLow, 0, sectorHeight, drawLow, drawHigh);
    const int portalDrawHigh = lerpi(portalNomHigh, 0, sectorHeight, drawLow, drawHigh);

    renderer_renderContourSpan(me, target, drawBoundary, x, drawLow, portalDrawLow);
    renderer_renderContourSpan(me, target, drawBoundary, x, portalDrawHigh, drawHigh);
}

void renderer_renderWall(Renderer* me, RendererTarget target, const DrawRequest* request, int wallIndex)
{
    const Sector* const sector = request->sector;
    const Wall* const wall = &sector->walls[wallIndex];

    lineSeg_t wallSeg = renderer_transformWall(me, sector, wallIndex);
    bool isWallStartBehind = real_compare(wallSeg.start.xz.z, real_zero) < 0;
    bool isWallEndBehind = real_compare(wallSeg.end.xz.z, real_zero) < 0;
    if (isWallStartBehind && isWallEndBehind)
        return;

    TexCoord texCoord = wall->texCoord;
    if (!renderer_clipByFov(me, &wallSeg, &texCoord))
        return;

    WallSection p;
    renderer_project(me, real_from_int(sector->heightOffset), real_from_int(sector->height), &wallSeg, &p);
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

    const Texture* texture = NULL;
    FilledSpanRenderFunc renderSpan;
    const void* renderSpanUser;
    if (wall->texture == INVALID_TEXTURE_ID) {
        renderSpan = renderer_renderFilledSpan_colored;
        renderSpanUser = &wall->color;
    } else {
        renderSpan = renderer_renderFilledSpan_textured;
        renderSpanUser = texture = texture_load(me->textureManager, wall->texture);
    }

    // render wall
    const BoundarySet* drawBoundary = &me->stackBoundarySets[request->depth];
    BoundarySet* nextBoundary = &me->stackBoundarySets[request->depth + 1];
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
    if (renderLeft == p.left.x && !isWallStartBehind && (wall->flags & WALL_CONTOUR_LEFT))
    {
        if (wall->portalTo < 0 || wall->flags & WALL_CONTOUR_LEFTPORTAL)
            renderer_renderContourSpan(me, target, drawBoundary, renderLeft, lowerIt.y0, upperIt.y0);
        else
            renderer_renderNonPortalContour(me, target, drawBoundary, renderLeft, portalNomStart, portalNomEnd, sector->height, lowerIt.y0, upperIt.y0);
    }
    if (renderRight == p.right.x && !isWallEndBehind && (wall->flags & WALL_CONTOUR_RIGHT))
    {
        if (wall->portalTo < 0 || wall->flags & WALL_CONTOUR_RIGHTPORTAL)
            renderer_renderContourSpan(me, target, drawBoundary, renderRight, lowerIt.y1, upperIt.y1);
        else
            renderer_renderNonPortalContour(me, target, drawBoundary, renderRight, portalNomStart, portalNomEnd, sector->height, lowerIt.y1, upperIt.y1);
    }

    // render wall spans
    BresenhamStep upperStep, lowerStep;
    int x = renderLeft;
    do {
        const int yBottom = drawBoundary->yBottom[x];
        const int yTop = drawBoundary->yTop[x];

        do {
            upperStep = bresenham_step(&upperIt);
            if (wall->flags & WALL_CONTOUR_TOP)
                renderer_renderContourSpan(me, target, drawBoundary, x, upperIt.y0, upperIt.y0);
        } while (upperStep != BRESENHAMSTEP_X && upperStep != BRESENHAMSTEP_NONE);
        do {
            lowerStep = bresenham_step(&lowerIt);
            if (wall->flags & WALL_CONTOUR_BOTTOM)
                renderer_renderContourSpan(me, target, drawBoundary, x, lowerIt.y0, lowerIt.y0);
        } while (lowerStep != BRESENHAMSTEP_X && lowerStep != BRESENHAMSTEP_NONE);
        assert(upperIt.x0 == lowerIt.x0);

        int yPortalStart = upperIt.y0, yPortalEnd = yTop + 1;
        if (wall->portalTo >= 0) {
            nextBoundary->yBottom[x] = yPortalStart = clampi(yBottom, lerpi(portalNomStart, 0, sector->height, lowerIt.y0, upperIt.y0), yTop);
            yPortalEnd = lerpi(portalNomEnd, 0, sector->height, lowerIt.y0, upperIt.y0);
            nextBoundary->yTop[x] = clampi(yBottom, yPortalEnd, yTop);

            if (wall->flags & WALL_CONTOUR_BOTTOMPORTAL)
                renderer_renderContourSpan(me, target, drawBoundary, x, yPortalStart, yPortalStart);
            if (wall->flags & WALL_CONTOUR_TOPPORTAL)
                renderer_renderContourSpan(me, target, drawBoundary, x, yPortalEnd, yPortalEnd);
        }

        me->wallBoundaries.yBottom[x] = max(yBottom, lowerIt.y0);
        me->wallBoundaries.yTop[x] = min(yTop, upperIt.y0 - 1);

        real_t xNorm = real_div(real_from_int(x - p.left.x), real_from_int(p.right.x - p.left.x));
        renderSpan(me, target, x,
            lowerIt.y0, upperIt.y0, max(yBottom, lowerIt.y0), min(yTop, yPortalStart - 1),
            xNorm, texCoord, &wallSeg, renderSpanUser);

        if (wall->portalTo >= 0) {
            renderSpan(me, target, x,
                lowerIt.y0, upperIt.y0, max(yBottom, yPortalEnd), min(yTop, upperIt.y0),
                xNorm, texCoord, &wallSeg, renderSpanUser);
        }

        x++;
    } while(upperStep != BRESENHAMSTEP_NONE);
    assert(lowerStep == BRESENHAMSTEP_NONE);

    if (texture != NULL)
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
    UNUSED(me, target, request);
    /*renderer_renderSlabColumns(me, target, request,
        me->wallBoundaries.yTop,
        me->boundarySets[me->curBoundarySet].yTop,
        true);
    renderer_renderSlabColumns(me, target, request,
        me->boundarySets[me->curBoundarySet].yBottom,
        me->wallBoundaries.yBottom,
        false);*/
}

void renderer_renderSpriteSpan(Renderer* me, RendererTarget target, const BoundarySet* drawBoundary, const Sprite* sprite, WallSection p, int x, real_t spriteYIncr)
{
    UNUSED(me);
    if (target.colorFormat != RendererColorFormat_1BitBW)
        return;
    uint8_t* fb = ((uint8_t*)target.framebuffer) + x * rendererColorFormat_getStride(target.colorFormat);

    const int texColumn = lerpi(x, p.left.x, p.right.x, 0, sprite->size.h - 1);
    assert(texColumn >= 0 && texColumn < sprite->size.h);
    const uint8_t* texBW = sprite->bw + texColumn * sprite->bytesPerRow;
    const uint8_t* texAlpha = sprite->alpha + texColumn * sprite->bytesPerRow;

    const int yLow = max(drawBoundary->yBottom[x], p.left.yStart);
    const int yHigh = min(drawBoundary->yTop[x], p.left.yEnd);
    real_t texRow = real_mul(real_from_int(yLow - p.left.yStart), spriteYIncr);

    for (int y = yLow; y <= yHigh; y++)
    {
        int texRowI = real_to_int(real_round(texRow));
        assert(texRowI >= 0 && texRowI < sprite->size.w);
        int texByte = texRowI / 8;
        int texShift = texRowI % 8;
        int fbByte = y / 8;
        int fbShift = y % 8;
        int fbMask = 1 << fbShift;

        int fbValue = fb[fbByte] >> fbShift;
        int texValue = texBW[texByte] >> texShift;
        int texAlphaValue = texAlpha[texByte] >> texShift;
        fbValue = (texAlphaValue & texValue) | (~texAlphaValue & fbValue);

        fb[fbByte] = (fb[fbByte] & ~fbMask) | ((fbValue & 1) << fbShift);

        texRow = real_add(texRow, spriteYIncr);
    }
}

void renderer_renderEntity(Renderer* me, RendererTarget target, const DrawRequest* request, int entityIndex)
{
    const Sector* const sector = request->sector;
    const Entity* const entity = &sector->entities[entityIndex];
    const BoundarySet* drawBoundary = &me->stackBoundarySets[request->depth];

    lineSeg_t entitySeg = renderer_transformEntity(me, entity->location.position, real_from_int(entity->radius));
    bool isEntityStartBehind = real_compare(entitySeg.start.xz.z, real_zero) < 0;
    bool isEntityEndBehind = real_compare(entitySeg.end.xz.z, real_zero) < 0;
    if (isEntityStartBehind && isEntityEndBehind)
        return;

    TexCoord texCoord = {
        .start = xy_zero,
        .end = xy_one
    };
    if (!renderer_clipByFov(me, &entitySeg, &texCoord))
        return;

    WallSection p;
    renderer_project(me, entity->location.height, real_from_int(entity->radius * 2), &entitySeg, &p);
    if (p.left.x >= p.right.x || p.right.x < request->left || p.left.x > request->right)
        return;
    // I wonder if we can run into precision errors here
    assert(p.left.yStart == p.right.yStart && p.left.yEnd == p.right.yEnd);

    const Sprite* sprite = sprite_load(me->textureManager, entity->sprite);
    const int renderLeft = max(request->left, p.left.x);
    const int renderRight = min(request->right, p.right.x);
    const real_t spriteYIncr = real_norm_lerp(real_from_int(p.left.yStart + 1),
        real_from_int(p.left.yStart), real_from_int(p.left.yEnd),
        real_zero, real_from_int(sprite->size.w - 1));

    for (int x = renderLeft; x <= renderRight; x++)
    {
        renderer_renderSpriteSpan(me, target, drawBoundary, sprite, p, x, spriteYIncr);
    }
    sprite_free(me->textureManager, sprite);
}

void renderer_renderSector(Renderer* renderer, RendererTarget target, const DrawRequest* request)
{
    for (int y = 0; y < RENDERER_HEIGHT; y++)
        renderer->spanStart[y] = -1;
    for (int i = 0; i < request->sector->wallCount; i++)
        renderer_renderWall(renderer, target, request, i);
    renderer_renderSlabs(renderer, target, request);
}

void renderer_renderSectorEntities(Renderer* renderer, RendererTarget target, const DrawRequest* request)
{
    for (int i = 0; i < request->sector->entityCount; i++)
    {
        if (request->sector->entities[i].sprite != INVALID_SPRITE_ID)
            renderer_renderEntity(renderer, target, request, i);
    }
}

void renderer_render(Renderer* renderer, RendererTarget target)
{
    target.framebuffer = ((uint8_t*)target.framebuffer) + (HUD_HEIGHT / 8);
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    for (int x = 0; x < RENDERER_WIDTH; x++)
    {
        memset(((uint8_t*)target.framebuffer) + x * stride, 0, stride - (HUD_HEIGHT / 8));
    }

    if (renderer->level == NULL || renderer->location.sector < 0)
        return;
    memset(renderer->transformedStatus, 0, sizeof(uint32_t) * ((renderer->level->vertexCount + 31) / 32));
    memset(renderer->stackBoundarySets[0].yBottom, 0, sizeof(renderer->stackBoundarySets[0].yBottom));
    memset(renderer->stackBoundarySets[0].yTop, RENDERER_HEIGHT - 1, sizeof(renderer->stackBoundarySets[0].yBottom));
    memset(renderer->wallBoundaries.yBottom, 0, sizeof(renderer->wallBoundaries.yBottom));
    memset(renderer->wallBoundaries.yTop, RENDERER_HEIGHT - 1, sizeof(renderer->wallBoundaries.yTop));

    drawRequestStack_reset(&renderer->drawRequests);
    drawRequestStack_push(&renderer->drawRequests,
        &renderer->level->sectors[renderer->location.sector],
        0, RENDERER_WIDTH - 1, NULL);
    //drawRequestStack_nextDepth(&renderer->drawRequests);

    int nextDepth = 0;
    for (int i = 0; i < renderer->drawRequests.count; i++)
    {
        if (i == nextDepth)
            drawRequestStack_nextDepth(&renderer->drawRequests);

        const DrawRequest* curRequest = &renderer->drawRequests.requests[i];
        renderer_renderSector(renderer, target, curRequest);

        if (i == nextDepth)
            nextDepth = renderer->drawRequests.count;
    }

    for (int i = renderer->drawRequests.count - 1; i >= 0; i--)
    {
        const DrawRequest* curRequest = &renderer->drawRequests.requests[i];
        renderer_renderSectorEntities(renderer, target, curRequest);
    }
}

Location* renderer_getLocation(Renderer* me)
{
    return &me->location;
}

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

void renderer_walk(Renderer* renderer, xz_t directions, int maxStepHeight)
{
    Location oldLocation = renderer->location;

    directions = xz_rotate(directions, real_neg(renderer->location.angle));
    renderer->location.position = xz_add(renderer->location.position, directions);
    if (!location_updateSectorNear(&renderer->location, renderer->level))
        return;

    if (renderer->location.sector < 0)
    {
        // wandered outside a wall
        renderer->location = oldLocation;
        return;
    }

    // gone through a portal
    Sector* oldSector = &renderer->level->sectors[oldLocation.sector];
    Sector* newSector = &renderer->level->sectors[renderer->location.sector];
    int stepSize = newSector->heightOffset - oldSector->heightOffset;
    if (stepSize > maxStepHeight)
    {
        renderer->location = oldLocation;
        return;
    }

    renderer->location.height = real_from_int(newSector->heightOffset);
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
    stack->count = stack->depth = 0;
}

void drawRequestStack_push(DrawRequestStack* stack, const Sector* sector, int left, int right, const Sector* sourceSector)
{
    const int insertAt = stack->count;
    assert(insertAt < MAX_DRAW_SECTORS);
    stack->count = (stack->count + 1) % MAX_DRAW_SECTORS;
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
