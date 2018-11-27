#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>
#include "cimgui.include.h"
#include "sdl.include.h"

typedef struct Window Window;
typedef void (*WindowUpdateCallback)(Window* window, void* userdata);
typedef struct WindowUpdateCallbacks
{
    WindowUpdateCallback
        before,
        content,
        after;
    void* userdata;
} WindowUpdateCallbacks;
typedef void (*WindowDragCallback)(Window* window, int mouseKey, ImVec2 delta, void* userdata);
typedef void (*WindowKeyCallback)(Window* window, SDL_Keysym sym, void* userdata);
typedef struct WindowKeyCallbacks
{
    WindowKeyCallback
        down,
        up;
    void* userdata;
} WindowKeyCallbacks;

typedef enum WindowOpenState
{
    WindowOpenState_Open,
    WindowOpenState_Closed,
    WindowOpenState_Unclosable
} WindowOpenState;

Uint32 getWindowIDByEvent(const SDL_Event* ev);

GRect window_getBounds(const Window* window);
bool window_isFocused(const Window* window);
WindowOpenState window_getOpenState(const Window* window);
void window_setTitle(Window* window, const char* title);
void window_setFlags(Window* window, ImGuiWindowFlags flags);
void window_setOpenState(Window* window, WindowOpenState state);
void window_setInitialBounds(Window* window, GRect bounds);
void window_setUpdateCallbacks(Window* window, WindowUpdateCallbacks callbacks);
void window_setDragCallback(Window* window, WindowDragCallback callback, void* userdata);
void window_setKeyCallbacks(Window* window, WindowKeyCallbacks callbacks);

typedef struct WindowContainer WindowContainer;
WindowContainer* windowContainer_init(GSize windowSize);
void windowContainer_free(WindowContainer* me);
void windowContainer_startUpdate(WindowContainer* me);
void windowContainer_endUpdate(WindowContainer* me);
Window* windowContainer_newWindow(WindowContainer* me, const char* title);
Window* windowContainer_getFocusedWindow(WindowContainer* me);
void windowContainer_handleEvent(WindowContainer* me, const SDL_Event* ev);

#endif
