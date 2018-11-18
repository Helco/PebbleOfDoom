#ifdef DEBUG_WINDOWS
#include "renderer_internal.h"
#include "platform.h"

int renderer_getDebugCount(Renderer* renderer)
{
    UNUSED(renderer);
    return 2;
}

const char* renderer_getDebugName(Renderer* renderer, int index)
{
    UNUSED(renderer);
    if (index < 0 || index > 1)
        return "";
    static const char* const NAMES[] = {
        "world-space",
        "player-space"
    };
    return NAMES[index];
}

void renderer_setDebugColor(const DebugInfo* debug, GColor color)
{
    SDL_SetRenderDrawColor(debug->ren, color.r * (255 / 3), color.g * (255 / 3), color.b * (255 / 3), 255);
}

void renderer_renderDebugLine(Renderer* me, const DebugInfo* debug, lineSeg_t line)
{
    lineSeg_t transformed = line;
    if (debug->index == 1)
        renderer_transformLine(me, &line, &transformed);
    xz_t start = xz_add(transformed.start.xz, debug->offset);
    xz_t end = xz_add(transformed.end.xz, debug->offset);
    SDL_RenderDrawLine(debug->ren,
        real_to_int(start.x), real_to_int(start.z),
        real_to_int(end.x), real_to_int(end.z));
}

void renderer_renderDebugVector(Renderer* me, const DebugInfo* debug, xz_t origin, xz_t direction)
{
    lineSeg_t seg;
    seg.start.xz = origin;
    seg.end.xz = xz_add(origin, direction);
    renderer_renderDebugLine(me, debug, seg);

    real_t arrowLength = real_div(xz_length(direction), real_from_int(3));
    direction = xz_normalize(direction);
    xz_t arrow = xz_scale(xz_rotate(direction, real_degToRad(160)), arrowLength);
    seg.start.xz = seg.end.xz;
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderDebugLine(me, debug, seg);

    arrow = xz_scale(xz_rotate(direction, real_degToRad(-160)), arrowLength);
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderDebugLine(me, debug, seg);
}

xz_t angleToVector(real_t angle, real_t length)
{
    return xz_scale(xz(
        real_cos(angle),
        real_sin(angle)
    ), length);
}

void renderer_renderDebug(Renderer* me, const DebugInfo* debug)
{
    const Wall* walls[] = {
        &me->wall,
        &me->wall2,
        &me->wall3,
        NULL
    };
    const Wall** curWall = walls;
    while (*curWall != NULL)
    {
        lineSeg_t wallLine;
        wallLine.start.xz = (*curWall)->start;
        wallLine.end.xz = (*curWall)->end;
        renderer_setDebugColor(debug, (*curWall)->wallColor);
        renderer_renderDebugLine(me, debug, wallLine);
        curWall++;
    }

    GColor red = GColorFromRGB(255, 0, 0);
    renderer_setDebugColor(debug, red);
    renderer_renderDebugVector(me, debug, me->location.position, angleToVector(me->location.angle, 30));
    lineSeg_t fovLine;
    fovLine.start.xz = me->location.position;
    fovLine.end.xz = xz_add(me->location.position, angleToVector(me->location.angle + me->halfFov, 70));
    renderer_renderDebugLine(me, debug, fovLine);
    fovLine.end.xz = xz_add(me->location.position, angleToVector(me->location.angle - me->halfFov, 70));
    renderer_renderDebugLine(me, debug, fovLine);
}

#endif
