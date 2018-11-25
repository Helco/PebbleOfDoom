#include "pcmockup.h"
#include <stdio.h>
#include <string.h>

#define CANARY_BUFFER_SIZE (2) // in framebuffers

struct PebbleWindow
{
    ImageWindow* window;
    SDL_Color* textureData;
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

    me->window = imageWindow_init("Pebble screen", (GSize) { initialBounds.w, initialBounds.h }, true);
    if (me->window == NULL)
    {
        pebbleWindow_free(me);
        return NULL;
    }
    imageWindow_setInitialPosition(me->window, (GPoint) { initialBounds.x, initialBounds.y });

    me->textureData = (SDL_Color*)malloc(sizeof(SDL_Color) * pebbleSize.w * pebbleSize.h);
    if (me->textureData == NULL)
    {
        fprintf(stderr, "Could not allocate pebble texture!\n");
        pebbleWindow_free(me);
        return NULL;
    }

    me->texturePixelFormat = SDL_AllocFormat(imageWindow_SDLPixelFormat);
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
    if (me->window != NULL)
        imageWindow_free(me->window);
    if (me->textureData != NULL)
        free(me->textureData);
    if (me->framebuffer != NULL)
        safeFramebuffer_free(me->framebuffer);
    if (me->texturePixelFormat != NULL)
        SDL_FreeFormat(me->texturePixelFormat);
    free(me);
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
        texPixels += me->pebbleSize.w;
    }
}

void pebbleWindow_startUpdate(PebbleWindow* me)
{
    safeFramebuffer_prepare(me->framebuffer);
}

void pebbleWindow_endUpdate(PebbleWindow* me)
{
    safeFramebuffer_check(me->framebuffer);
    prv_pebbleWindow_convertPebbleToTexture(me);
    imageWindow_setImageData(me->window, me->pebbleSize, me->textureData);
    imageWindow_update(me->window);
}

GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window)
{
    return safeFramebuffer_getScreenBuffer(window->framebuffer);
}

ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* me)
{
    return me->window;
}
