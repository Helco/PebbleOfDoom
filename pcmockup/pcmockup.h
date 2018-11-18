#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include <SDL.h>
#include "pebble.h"
#include "../renderer/renderer.h"

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
DebugWindow* debugWindow_init(SDL_Rect bounds, int index, const char* title);
void debugWindow_free(DebugWindow* window);
void debugWindow_startUpdate(DebugWindow* window);
void debugWindow_endUpdate(DebugWindow* window);
void debugWindow_handleEvent(DebugWindow* window, const SDL_Event* ev);
const DebugInfo* debugWindow_getDebugInfo(DebugWindow* window);

typedef struct DebugWindowSet DebugWindowSet;
DebugWindowSet* debugWindowSet_init(const WindowGrid* grid, Renderer* renderer);
void debugWindowSet_free(DebugWindowSet* set);
void debugWindowSet_update(DebugWindowSet* set);
void debugWindowSet_handleUpdate(DebugWindowSet* set, const SDL_Event* ev);

#endif
