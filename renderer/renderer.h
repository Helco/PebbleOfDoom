#ifndef RENDERER_H
#define RENDERER_H
#include <pebble.h>
#include "algebra.h"

#define RENDERER_WIDTH 168
#define RENDERER_HEIGHT 144

typedef struct Renderer Renderer;
Renderer* renderer_init();
void renderer_free(Renderer* renderer);
/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer);

#ifdef DEBUG_WINDOWS
#include <SDL.h>

typedef struct DebugInfo
{
    int index;
    SDL_Renderer* ren;
    xz_t offset;
} DebugInfo;

int renderer_getDebugCount(Renderer* renderer);
const char* renderer_getDebugName(Renderer* renderer, int index);
void renderer_renderDebug(Renderer* me, const DebugInfo* debug);
#endif

#endif
