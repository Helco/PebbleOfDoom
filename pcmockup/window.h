#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>
#include "cimgui.include.h"
#include "sdl.include.h"

typedef struct Window Window;
typedef void (*WindowDestructorCallback)(void* userdata);
typedef void (*WindowUpdateCallback)(void* userdata);
typedef void (*WindowDragCallback)(int mouseKey, ImVec2 delta, void* userdata);
typedef void (*WindowKeyCallback)(SDL_Keysym sym, void* userdata);
typedef struct WindowCallbacks {
    uint32_t tag;
    void* userdata;
    WindowDestructorCallback destruct;
    WindowUpdateCallback
        beforeUpdate,
        contentUpdate,
        afterUpdate,
        mainMenubar;
    WindowDragCallback drag;
    WindowKeyCallback
        keyDown,
        keyUp;
} WindowCallbacks;

typedef enum WindowOpenState
{
    WindowOpenState_Open,
    WindowOpenState_Closed,
    WindowOpenState_Unclosable
} WindowOpenState;

Uint32 getWindowIDByEvent(const SDL_Event* ev);

void window_free(Window* window);
GRect window_getBounds(const Window* window);
bool window_isFocused(const Window* window);
WindowOpenState window_getOpenState(const Window* window);
const char* window_getMenubarSection(const Window* window);
bool window_hasTag(const Window* window, uint32_t tag);
void window_setTitle(Window* window, const char* title);
void window_setFlags(Window* window, ImGuiWindowFlags flags);
void window_setOpenState(Window* window, WindowOpenState state);
void window_setInitialBounds(Window* window, GRect bounds);
void window_setMenubarSection(Window* window, const char* section);
void window_addCallbacks(Window* window, WindowCallbacks callbacks);
void window_updateMenubar(Window* window);

typedef struct WindowContainer WindowContainer;
WindowContainer* windowContainer_init(GSize windowSize);
void windowContainer_free(WindowContainer* me);
void windowContainer_update(WindowContainer* me);
Window* windowContainer_newWindow(WindowContainer* me, const char* title);
Window* windowContainer_getFocusedWindow(WindowContainer* me);
void windowContainer_handleEvent(WindowContainer* me, const SDL_Event* ev);
void windowContainer_addMenubarHandler(WindowContainer* me, WindowUpdateCallback callback,
    const char* section, void* userdata);
int windowContainer_getWindowCount(const WindowContainer* me);
Window* windowContainer_getWindowByIndex(WindowContainer* me, int index);

typedef struct WindowGrid
{
    int windowCount;
    GSize totalSize;
} WindowGrid;
GSize windowGrid_getGridSize(const WindowGrid* grid);
GRect windowGrid_getSingleBounds(const WindowGrid* grid, int windowI); // negative to select from end

#endif
