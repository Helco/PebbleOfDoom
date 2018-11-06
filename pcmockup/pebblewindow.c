#include "pcmockup.h"
#include <stdio.h>
#include <string.h>

struct PebbleWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* pebbleTexture;
    SDL_PixelFormat* texturePixelFormat;
    GColor* pebbleFramebuffer;
    GSize pebbleSize;
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

    this->texturePixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    if (this->texturePixelFormat == NULL)
    {
        fprintf(stderr, "SDL_AllocFormat: %s\n", SDL_GetError());
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

    this->pebbleSize = pebbleSize;

    return this;
}

void pebbleWindow_free(PebbleWindow* this)
{
    if (this == NULL)
        return;
    if (this->pebbleFramebuffer != NULL)
        free(this->pebbleFramebuffer);
    if (this->texturePixelFormat != NULL)
        SDL_FreeFormat(this->texturePixelFormat);
    if (this->pebbleTexture != NULL)
        SDL_DestroyTexture(this->pebbleTexture);
    if (this->renderer != NULL)
        SDL_DestroyRenderer(this->renderer);
    if (this->window != NULL)
        SDL_DestroyWindow(this->window);
    free(this);
}

static SDL_Rect prv_pebbleWindow_fitPebbleScreen(const PebbleWindow* this)
{
    SDL_Rect src;
    SDL_GetWindowSize(this->window, &src.w, &src.h);
    const float pebbleAspect = (float)this->pebbleSize.w / this->pebbleSize.h;
    return findBestFit(src, pebbleAspect);
}

static inline SDL_Color prv_convertGColorTo32Bit(GColor pebbleColor)
{
    SDL_Color color;
    color.r = pebbleColor.r * (255 / 3);
    color.g = pebbleColor.g * (255 / 3);
    color.b = pebbleColor.b * (255 / 3);
    color.a = 255;
    return color;
}

static void prv_pebbleWindow_convertPebbleToTexture(PebbleWindow* this)
{
    const GColor* pebblePixels = this->pebbleFramebuffer;
    char* texPixels;
    int texPitch;
    SDL_LockTexture(this->pebbleTexture, NULL, (void**)&texPixels, &texPitch);

    uint32_t* itTexPixel;
    const GColor* itPebblePixel;
    for (int y = 0; y < this->pebbleSize.h; y++)
    {
        itTexPixel = (uint32_t*)texPixels;
        for (int x = 0; x < this->pebbleSize.w; x++)
        {
            itPebblePixel = pebblePixels + x * this->pebbleSize.h + y;
            SDL_Color color = prv_convertGColorTo32Bit(*itPebblePixel);
            *itTexPixel = SDL_MapRGBA(this->texturePixelFormat,
                color.r, color.g, color.b, color.a);

            itTexPixel++;
        }

        // Advance to next line
        texPixels += texPitch;
    }

    SDL_UnlockTexture(this->pebbleTexture);
}

void pebbleWindow_update(PebbleWindow* this)
{
    prv_pebbleWindow_convertPebbleToTexture(this);

    SDL_SetRenderDrawColor(this->renderer, 255, 0, 255, 255);
    SDL_RenderClear(this->renderer);
    const SDL_Rect dst = prv_pebbleWindow_fitPebbleScreen(this);
    SDL_RenderCopy(this->renderer, this->pebbleTexture, NULL, &dst);
    SDL_RenderPresent(this->renderer);
}

SDL_Rect pebbleWindow_getBounds(PebbleWindow* me)
{
    SDL_Rect bounds;
    SDL_GetWindowPosition(me->window, &bounds.x, &bounds.y);
    SDL_GetWindowSize(me->window, &bounds.w, &bounds.h);
    return bounds;
}

GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window)
{
    return window->pebbleFramebuffer;
}
