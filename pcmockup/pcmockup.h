#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include "sdl.include.h"
#include "pebble.h"
#include "renderer.h"
#include "window.h"

SDL_Rect findBestFit(SDL_Rect target, float aspect);
SDL_Rect padRect(SDL_Rect rect, GSize amount);

extern const Uint32 imageWindow_SDLPixelFormat;
typedef struct ImageWindow ImageWindow;
ImageWindow* imageWindow_init(WindowContainer* parent, const char* title, GRect initialBounds, bool_t isEssential);
void imageWindow_free(ImageWindow* me);
void imageWindow_setImageData(ImageWindow* me, GSize size, const SDL_Color* data);
void imageWindow_toggle(ImageWindow* me, bool_t isOpen);
bool_t imageWindow_isOpen(ImageWindow* me);
Window* imageWindow_asWindow(ImageWindow* me);

typedef struct WindowGrid
{
    int windowCount;
    GSize totalSize;
} WindowGrid;
GSize windowGrid_getGridSize(const WindowGrid* grid);
SDL_Rect windowGrid_getSingleBounds(const WindowGrid* grid, int windowI); // negative to select from end

typedef struct SafeFramebuffer SafeFramebuffer;
SafeFramebuffer* safeFramebuffer_init(GSize size, int canarySize); // canarySize in framebuffer sizes
void safeFramebuffer_free(SafeFramebuffer* me);
GColor* safeFramebuffer_getScreenBuffer(SafeFramebuffer* me);
void safeFramebuffer_prepare(SafeFramebuffer* me);
void safeFramebuffer_check(SafeFramebuffer* me);

typedef struct PebbleWindow PebbleWindow;
PebbleWindow* pebbleWindow_init(WindowContainer* parent, SDL_Rect initialBounds, GSize pebbleSize, Renderer* renderer);
void pebbleWindow_free(PebbleWindow* window);
void pebbleWindow_startUpdate(PebbleWindow* me);
void pebbleWindow_endUpdate(PebbleWindow* me);
GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window);
ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* window);

typedef struct DebugWindow DebugWindow;
DebugWindow* debugWindow_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* debugView, Renderer* renderer);
void debugWindow_free(DebugWindow* window);
void debugWindow_update(DebugWindow* window);
const DebugView* debugWindow_getDebugView(const DebugWindow* window);
ImageWindow* debugWindow_asImageWindow(DebugWindow* window);

typedef struct DebugWindowSet DebugWindowSet;
DebugWindowSet* debugWindowSet_init(WindowContainer* parent, const WindowGrid* grid, Renderer* renderer);
void debugWindowSet_free(DebugWindowSet* set);
void debugWindowSet_update(DebugWindowSet* set);
void debugWindowSet_updateMenubar(DebugWindowSet* me);

typedef struct PCMockup PCMockup;
PCMockup* pcmockup_init();
void pcmockup_free(PCMockup* me);
void pcmockup_update(PCMockup* me);
void pcmockup_mainLoop(PCMockup* me);

typedef struct TextureManager TextureManager;
TextureManager* textureManager_init();
void textureManager_free(TextureManager* me); // asserts that all textures are freed
TextureId textureManager_registerFile(TextureManager* me, const char* filename); // relative to texture folder
const Texture* textureManager_loadTexture(TextureManager* me, TextureId id);
const Texture* textureManager_createEmptyTexture(TextureManager* me, GSize size, GColor** outputPtr);
void textureManager_freeTexture(TextureManager* me, const Texture* texture);

#endif
