#ifndef WINDOW_INTERNAL_H
#define WINDOW_INTERNAL_H
#include "window.h"

Window* window_init(WindowContainer* parent);
void window_free(Window* window);
void window_update(Window* window);
void window_handleDragEvent(Window* window);
void window_handleKeyEvent(Window* window, SDL_Keysym sym, bool isDown);

char** windowContainer_getMenubarSectionPtr(WindowContainer* me, const Window* window);
void windowContainer_addMenubarHandlerWithWindow(WindowContainer* me, WindowUpdateCallback callback,
    Window* window, const char* section, void* userdata);

#endif
