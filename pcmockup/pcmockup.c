#include <SDL2/SDL.h>
#include <stdio.h>
#include "pcmockup.h"
#include "../renderer/renderer.h"


static const int START_WINDOW_WIDTH = RENDERER_WIDTH * 4;
static const int START_WINDOW_HEIGHT = RENDERER_HEIGHT * 4;

#undef main
int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    PebbleWindow* pebbleWindow = pebbleWindow_init(
        GSize(START_WINDOW_WIDTH, START_WINDOW_HEIGHT),
        GSize(RENDERER_WIDTH, RENDERER_HEIGHT)
    );
    if (pebbleWindow == NULL)
        return -1;
    Renderer* renderer = renderer_init();
    if (renderer == NULL)
        return -1;
        
    // Look carefully for I will render this only once
    GColor* framebuffer = pebbleWindow_getPebbleFramebuffer(pebbleWindow);
    renderer_render(renderer, framebuffer);
    pebbleWindow_update(pebbleWindow);

    pebbleWindow_free(pebbleWindow);
    SDL_Quit();
    return 0;
}
