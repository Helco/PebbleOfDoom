#include "pcmockup.h"
#include <stdio.h>
#include <string.h>

#define CANARY_BUFFER_SIZE (2) // in framebuffers

struct PebbleWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* pebbleTexture;
    SDL_PixelFormat* texturePixelFormat;
    SafeFramebuffer* framebuffer;
    GSize pebbleSize;
};

PebbleWindow* pebbleWindow_init(SDL_Rect initialBounds, GSize pebbleSize)
{
    PebbleWindow* me = (PebbleWindow*)malloc(sizeof(PebbleWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(PebbleWindow));

    me->window = SDL_CreateWindow("PebbleOfDoom - PCMockup",
        initialBounds.x, initialBounds.y,
        initialBounds.w, initialBounds.h,
        SDL_WINDOW_RESIZABLE);
    if (me->window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        pebbleWindow_free(me);
        return NULL;
    }

    me->renderer = SDL_CreateRenderer(me->window, -1, 0);
    if (me->renderer == NULL)
    {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        pebbleWindow_free(me);
        return NULL;
    }

    me->pebbleTexture = SDL_CreateTexture(me->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        pebbleSize.w, pebbleSize.h);
    if (me->pebbleTexture == NULL)
    {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        pebbleWindow_free(me);
        return NULL;
    }

    me->texturePixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    if (me->texturePixelFormat == NULL)
    {
        fprintf(stderr, "SDL_AllocFormat: %s\n", SDL_GetError());
        pebbleWindow_free(me);
        return NULL;
    }

    me->framebuffer = safeFramebuffer_init(pebbleSize, CANARY_BUFFER_SIZE);
    if (me->framebuffer == NULL)
    {
        pebbleWindow_free(me);
        return NULL;
    }

    me->pebbleSize = pebbleSize;

    return me;
}

void pebbleWindow_free(PebbleWindow* me)
{
    if (me == NULL)
        return;
    if (me->framebuffer != NULL)
        safeFramebuffer_free(me->framebuffer);
    if (me->texturePixelFormat != NULL)
        SDL_FreeFormat(me->texturePixelFormat);
    if (me->pebbleTexture != NULL)
        SDL_DestroyTexture(me->pebbleTexture);
    if (me->renderer != NULL)
        SDL_DestroyRenderer(me->renderer);
    if (me->window != NULL)
        SDL_DestroyWindow(me->window);
    free(me);
}

static SDL_Rect prv_pebbleWindow_fitPebbleScreen(const PebbleWindow* me)
{
    SDL_Rect src;
    SDL_GetWindowSize(me->window, &src.w, &src.h);
    const float pebbleAspect = (float)me->pebbleSize.w / me->pebbleSize.h;
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

static void prv_pebbleWindow_convertPebbleToTexture(PebbleWindow* me)
{
    const GColor* pebblePixels = pebbleWindow_getPebbleFramebuffer(me);
    char* texPixels;
    int texPitch;
    SDL_LockTexture(me->pebbleTexture, NULL, (void**)&texPixels, &texPitch);

    uint32_t* itTexPixel;
    const GColor* itPebblePixel;
    for (int y = 0; y < me->pebbleSize.h; y++)
    {
        itTexPixel = (uint32_t*)texPixels;
        for (int x = 0; x < me->pebbleSize.w; x++)
        {
            itPebblePixel = pebblePixels + x * me->pebbleSize.h + y;
            SDL_Color color = prv_convertGColorTo32Bit(*itPebblePixel);
            *itTexPixel = SDL_MapRGBA(me->texturePixelFormat,
                color.r, color.g, color.b, color.a);

            itTexPixel++;
        }

        // Advance to next line
        texPixels += texPitch;
    }

    SDL_UnlockTexture(me->pebbleTexture);
}

void pebbleWindow_startUpdate(PebbleWindow* me)
{
    safeFramebuffer_prepare(me->framebuffer);
}

void pebbleWindow_endUpdate(PebbleWindow* me)
{
    safeFramebuffer_check(me->framebuffer);
    prv_pebbleWindow_convertPebbleToTexture(me);

    SDL_SetRenderDrawColor(me->renderer, 255, 0, 255, 255);
    SDL_RenderClear(me->renderer);
    const SDL_Rect dst = prv_pebbleWindow_fitPebbleScreen(me);
    SDL_RenderCopy(me->renderer, me->pebbleTexture, NULL, &dst);
    SDL_RenderPresent(me->renderer);
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
    return safeFramebuffer_getScreenBuffer(window->framebuffer);
}
