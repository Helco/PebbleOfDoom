#ifdef DEBUG_WINDOWS
#include "renderer.h"

int renderer_getDebugCount(Renderer* renderer)
{
    return 2;
}

const char* renderer_getDebugName(Renderer* renderer, int index)
{
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

void renderer_renderDebug(Renderer* me, const DebugInfo* debug)
{
    const GColor red = { 0b00001111 };
    const GColor blue = { 0b11000011 };
    renderer_setDebugColor(debug, debug->index == 0 ? red : blue);
    SDL_RenderClear(debug->ren);
}

#endif
