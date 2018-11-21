#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include "sdl.include.h"
#include "pebble.h"
#include "renderer.h"

SDL_Rect findBestFit(SDL_Rect target, float aspect);
SDL_Rect padRect(SDL_Rect rect, GSize amount);

typedef struct WindowGrid
{
    int windowCount;
    GSize totalSize;
} WindowGrid;
GSize windowGrid_getGridSize(const WindowGrid* grid);
SDL_Rect windowGrid_getSingleBounds(const WindowGrid* grid, int windowI); // negative to select from end

typedef struct PebbleWindow PebbleWindow;
PebbleWindow* pebbleWindow_init(SDL_Rect initialBounds, GSize pebbleSize);
void pebbleWindow_free(PebbleWindow* window);
void pebbleWindow_update(PebbleWindow* window);
SDL_Rect pebbleWindow_getBounds(PebbleWindow* window);
GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window);

typedef struct DebugWindow DebugWindow;
DebugWindow* debugWindow_init(SDL_Rect bounds, const DebugView* debugView, Renderer* renderer);
void debugWindow_free(DebugWindow* window);
void debugWindow_update(DebugWindow* window);
void debugWindow_handleEvent(DebugWindow* window, const SDL_Event* ev);

typedef struct DebugWindowSet DebugWindowSet;
DebugWindowSet* debugWindowSet_init(const WindowGrid* grid, Renderer* renderer);
void debugWindowSet_free(DebugWindowSet* set);
void debugWindowSet_update(DebugWindowSet* set);
void debugWindowSet_handleEvent(DebugWindowSet* set, const SDL_Event* ev);

typedef struct PCMockup PCMockup;
PCMockup* pcmockup_init();
void pcmockup_free(PCMockup* me);
void pcmockup_update(PCMockup* me);
void pcmockup_mainLoop(PCMockup* me);

#endif
