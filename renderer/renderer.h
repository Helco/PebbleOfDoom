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

typedef enum RendererColorFormat
{
    RendererColorFormat_8BitColor,
    RendererColorFormat_1BitBW
} RendererColorFormat;
int rendererColorFormat_getStride(RendererColorFormat format);

typedef enum TextureMappingMode
{
    TextureMappingMode_Perspective = 0,
    TextureMappingMode_Affine
} TextureMappingMode;

typedef struct RendererTarget
{
    void* framebuffer;
    RendererColorFormat colorFormat;
} RendererTarget;

typedef struct Renderer Renderer;
Renderer* renderer_init();
void renderer_free(Renderer* renderer);
void renderer_setLevel(Renderer* renderer, const Level* level);
void renderer_setTextureManager(Renderer* renderer, TextureManagerHandle handle);
void renderer_setTextureMappingMode(Renderer* renderer, TextureMappingMode mode);
/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, const RendererTarget target);

void renderer_moveTo(Renderer* renderer, Location playerLocation);
void renderer_rotate(Renderer* renderer, real_t angle); //angle should be in degrees
void renderer_move(Renderer* renderer, xz_t direction);
void renderer_moveVertical(Renderer* renderer, xy_t direction);

#ifdef DEBUG_WINDOWS
struct SDL_Renderer; // no need to include SDL here
typedef int ImGuiWindowFlags;

typedef enum DebugViewType
{
    DebugViewType_SDL,
    DebugViewType_ImGui
} DebugViewType;

typedef void (*DebugViewCallback_SDL)(Renderer* me, struct SDL_Renderer* sdlRenderer, xz_t offset, const void* userdata);
typedef void (*DebugViewCallback_ImGui)(Renderer* me, ImGuiWindowFlags* flags, const void* userdata);

typedef struct DebugView
{
    const char* name;
    DebugViewType type;
    union {
        DebugViewCallback_SDL sdl;
        DebugViewCallback_ImGui imgui;
    } callback;
    bool_t startsOpened;
    const void* userdata; // given to the callbacks
} DebugView;

int renderer_getDebugCount(const Renderer* renderer);
const DebugView* renderer_getDebugViews(const Renderer* renderer);
#endif

#endif
