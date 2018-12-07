#ifndef RENDERER_H
#define RENDERER_H
#include <pebble.h>
#include "algebra.h"
#include "level.h"
#include "texture.h"

#define RENDERER_WIDTH 168
#define HALF_RENDERER_WIDTH (RENDERER_WIDTH / 2)
#define RENDERER_HEIGHT 144
#define HALF_RENDERER_HEIGHT (RENDERER_HEIGHT / 2)

typedef struct {
    xz_t position;
    real_t height;
    real_t angle;
} Location;

enum angle {rotateLeft = -1, rotateRight = 1};

typedef struct Renderer Renderer;
Renderer* renderer_init();
void renderer_free(Renderer* renderer);
void renderer_setLevel(Renderer* renderer, const Level* level);
void renderer_setTextureManager(Renderer* renderer, TextureManagerHandle handle);
/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer);

void renderer_moveTo(Renderer* renderer, Location playerLocation);
void renderer_rotate(Renderer* renderer, int angle); //angle should be in degrees
void renderer_move(Renderer* renderer, xz_t directions);
void renderer_moveHorizontal(Renderer* renderer, xy_t directions);

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
