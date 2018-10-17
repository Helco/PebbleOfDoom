#include "pcmockup.h"

struct PebbleWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* pebbleTexture;
    GColor* pebbleFramebuffer;
};

PebbleWindow* pebbleWindow_init(GSize windowSize, GSize pebbleSize)
{
    PebbleWindow* this = (PebbleWindow*)malloc(sizeof(PebbleWindow));
    if (this == NULL)
        return NULL;
    memset(this, 0, sizeof(PebbleWindow));
    
    this->window = SDL_CreateWindow("PebbleOfDoom - PCMockup",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowSize.w, windowSize.h,
        SDL_WINDOW_RESIZABLE);
    if (this->window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        pebbleWindow_free(this);
        return NULL;
    }

    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    if (this->renderer == NULL)
    {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        pebbleWindow_free(this);
        return NULL;
    }

    this->pebbleTexture = SDL_CreateTexture(this->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        pebbleSize.w, pebbleSize.h);
    if (this->pebbleTexture == NULL)
    {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        pebbleWindow_free(this);
        return NULL;
    }

    this->pebbleFramebuffer = (GColor*)malloc(sizeof(GColor) * pebbleSize.w * pebbleSize.h);
    if (this->pebbleFramebuffer == NULL)
    {
        fprintf(stderr, "Could not allocate pebble framebuffer!\n");
        pebbleWindow_free(this);
        return NULL;
    }

    return this;
}

void pebbleWindow_free(PebbleWindow* this)
{
    if (this == NULL)
        return;
    if (this->pebbleFramebuffer != NULL)
        free(this->pebbleFramebuffer);
    if (this->pebbleTexture != NULL)
        SDL_DestroyTexture(this->pebbleTexture);
    if (this->renderer != NULL)
        SDL_DestroyRenderer(this->renderer);
    if (this->window != NULL)
        SDL_DestroyWindow(this->window);
    free(this);
}

void pebbleWindow_update(PebbleWindow* window)
{

}

GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window)
{
    return window->pebbleFramebuffer;
}
