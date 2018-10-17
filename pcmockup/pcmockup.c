#include <SDL2/SDL.h>
#include <stdio.h>
#include "pcmockup.h"
#include "../renderer/renderer.h"

static const int START_WINDOW_WIDTH = RENDERER_WIDTH * 2;
static const int START_WINDOW_HEIGHT = RENDERER_HEIGHT * 2;
static const int MAX_FRAMERATE = 60;

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
        
    GColor* framebuffer = pebbleWindow_getPebbleFramebuffer(pebbleWindow);
    renderer_render(renderer, framebuffer);

    int isRunning = 1;
    while (isRunning)
    {
        const uint32_t frameStart = SDL_GetTicks();

        pebbleWindow_update(pebbleWindow);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                isRunning = 0;
            else if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case (SDLK_ESCAPE): { isRunning = 0; }break;
                    case (SDLK_SPACE):
                    {
                        renderer_render(renderer, framebuffer);
                    }break;
                }
            }
        }

        const uint32_t frameEnd = SDL_GetTicks();
        const int delay = (1000 / MAX_FRAMERATE) - (int)(frameEnd - frameStart);
        if (delay > 0)
            SDL_Delay(delay);
    }

    renderer_free(renderer);
    pebbleWindow_free(pebbleWindow);
    SDL_Quit();
    return 0;
}
