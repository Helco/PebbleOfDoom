#ifdef DEBUG_WINDOWS
#include "renderer_internal.h"
#include "platform.h"
#include "sdl.include.h"

typedef struct
{
    bool_t transform;
} RenderWorldOptions;
static const RenderWorldOptions worldSpaceOptions = {
    .transform = false
};
static const RenderWorldOptions playerSpaceOptions = {
    .transform = true
};

void renderer_setDebugColor(SDL_Renderer* sdlRenderer, GColor color)
{
    SDL_SetRenderDrawColor(sdlRenderer, color.r * (255 / 3), color.g * (255 / 3), color.b * (255 / 3), 255);
}

void renderer_renderSDLDebugLine(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, lineSeg_t line, const RenderWorldOptions* opts)
{
    lineSeg_t transformed = line;
    if (opts->transform)
        renderer_transformLine(me, &line, &transformed);
    xz_t start = xz_add(transformed.start.xz, offset);
    xz_t end = xz_add(transformed.end.xz, offset);
    SDL_RenderDrawLine(sdlRenderer,
        real_to_int(start.x), real_to_int(start.z),
        real_to_int(end.x), real_to_int(end.z));
}

void renderer_renderDebugVector(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, xz_t origin, xz_t direction, const RenderWorldOptions* opts)
{
    lineSeg_t seg;
    seg.start.xz = origin;
    seg.end.xz = xz_add(origin, direction);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);

    real_t arrowLength = real_div(xz_length(direction), real_from_int(3));
    direction = xz_normalize(direction);
    xz_t arrow = xz_scale(xz_rotate(direction, real_degToRad(real_from_int(160))), arrowLength);
    seg.start.xz = seg.end.xz;
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);

    arrow = xz_scale(xz_rotate(direction, real_degToRad(real_from_int(-160))), arrowLength);
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);
}

xz_t angleToVector(real_t angle, real_t length)
{
    return xz_scale(xz(
        real_cos(angle),
        real_sin(angle)
    ), length);
}

void renderer_renderDebugSector(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, const Sector* sector, const RenderWorldOptions* opts)
{
    const Wall* curWall = sector->walls;
    for (int i = 0; i < sector->wallCount; i++, curWall++)
    {
        lineSeg_t wallLine;
        wallLine.start.xz = curWall->startCorner;
        wallLine.end.xz = sector->walls[(i + 1) % sector->wallCount].startCorner;
        renderer_setDebugColor(sdlRenderer, curWall->color);
        renderer_renderSDLDebugLine(me, sdlRenderer, offset, wallLine, opts);
    }
}

void renderer_debug_renderWorld(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, const void* userdata)
{
    const RenderWorldOptions* opts = (const RenderWorldOptions*)userdata;
    const Sector* curSector = me->level->sectors;
    for (int i = 0; i < me->level->sectorCount; i++, curSector++)
        renderer_renderDebugSector(me, sdlRenderer, offset, curSector, opts);

    GColor red = GColorFromRGB(255, 0, 0);
    renderer_setDebugColor(sdlRenderer, red);
    renderer_renderDebugVector(me, sdlRenderer, offset, me->location.position, angleToVector(me->location.angle, real_from_int(30)), opts);
    lineSeg_t fovLine;
    fovLine.start.xz = me->location.position;
    fovLine.end.xz = xz_add(me->location.position, angleToVector(real_add(me->location.angle, me->halfFov), real_from_int(70)));
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, fovLine, opts);
    fovLine.end.xz = xz_add(me->location.position, angleToVector(real_sub(me->location.angle, me->halfFov), real_from_int(70)));
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, fovLine, opts);
}

static const DebugView debugViews[] = {
    {
        .name = "world-space",
        .type = DebugViewType_SDL,
        .callback = { .sdl = renderer_debug_renderWorld },
        .userdata = &worldSpaceOptions
    },
    {
        .name = "player-space",
        .type = DebugViewType_SDL,
        .callback = { .sdl = renderer_debug_renderWorld },
        .userdata = &playerSpaceOptions
    }
};

int renderer_getDebugCount(const Renderer* renderer)
{
    UNUSED(renderer);
    return sizeof(debugViews) / sizeof(DebugView);
}

const DebugView* renderer_getDebugViews(const Renderer* renderer)
{
    UNUSED(renderer);
    return debugViews;
}

#endif
