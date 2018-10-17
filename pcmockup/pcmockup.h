#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include <SDL2/SDL.h>
#include "pebble.h"

typedef struct PebbleWindow PebbleWindow;
PebbleWindow* pebbleWindow_init(GSize windowSize, GSize pebbleSize);
void pebbleWindow_free(PebbleWindow* window);
void pebbleWindow_update(PebbleWindow* window);
GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window);

#endif
