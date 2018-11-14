#ifndef RENDERER_H
#define RENDERER_H
#include <pebble.h>
#include "algebra.h"

#define RENDERER_WIDTH 168
#define HALF_RENDERER_WIDTH (RENDERER_WIDTH / 2)
#define RENDERER_HEIGHT 144
#define HALF_RENDERER_HEIGHT (RENDERER_HEIGHT / 2)

typedef struct {
    xz_t position;
    real_t height;
    real_t angle;
} Location;

typedef struct Renderer Renderer;
Renderer* renderer_init();
void renderer_free(Renderer* renderer);
/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer);

void renderer_renderNewRelativeLocation(Renderer* renderer, GColor* framebuffer, Location playerLocation);
void renderer_renderRotateRight(Renderer* renderer, GColor* framebuffer);
void renderer_renderRotateLeft(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveRight(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveLeft(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveUp(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveDown(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveForward(Renderer* renderer, GColor* framebuffer);
void renderer_renderMoveBackwards(Renderer* renderer, GColor* framebuffer);

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
