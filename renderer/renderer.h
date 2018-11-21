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

void renderer_moveTo(Renderer* renderer, Location playerLocation);
void renderer_rotateRight(Renderer* renderer);
void renderer_rotateLeft(Renderer* renderer);
void renderer_moveRight(Renderer* renderer);
void renderer_moveLeft(Renderer* renderer);
void renderer_moveUp(Renderer* renderer);
void renderer_moveDown(Renderer* renderer);
void renderer_moveForward(Renderer* renderer);
void renderer_moveBackwards(Renderer* renderer);

#ifdef DEBUG_WINDOWS
struct SDL_Renderer; // no need to include SDL here

typedef enum DebugViewType
{
    DebugViewType_SDL
} DebugViewType;

typedef void (*DebugViewCallback_SDL)(Renderer* me, struct SDL_Renderer* sdlRenderer, xz_t offset, const void* userdata);

typedef struct DebugView
{
    const char* name;
    DebugViewType type;
    union {
        DebugViewCallback_SDL sdl;
    } callback;
    const void* userdata; // given to the callbacks
} DebugView;

int renderer_getDebugCount(const Renderer* renderer);
const DebugView* renderer_getDebugViews(const Renderer* renderer);
#endif

#endif
