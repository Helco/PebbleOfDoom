#include "pcmockup.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>

#define CANARY_BUFFER_SIZE (2) // in framebuffers

struct PebbleWindow
{
    ImageWindow* window;
    SDL_Color* textureData;
    SDL_PixelFormat* texturePixelFormat;
    SafeFramebuffer* framebuffer;
    Renderer* renderer;
    GSize pebbleSize;
};

void pebbleWindow_onKeyDown(Window* window, SDL_Keysym sym, void* userdata);

PebbleWindow* pebbleWindow_init(WindowContainer* parent, SDL_Rect initialBounds, GSize pebbleSize, Renderer* renderer)
{
    PebbleWindow* me = (PebbleWindow*)malloc(sizeof(PebbleWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(PebbleWindow));

    GRect b = { { initialBounds.x, initialBounds.y }, { initialBounds.w, initialBounds.h } };
    me->window = imageWindow_init(parent, "Pebble screen", b, true);
    if (me->window == NULL)
    {
        pebbleWindow_free(me);
        return NULL;
    }
    window_setKeyCallbacks(imageWindow_asWindow(me->window), (WindowKeyCallbacks) {
        .down = pebbleWindow_onKeyDown,
        .userdata = me
    });

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
    me->renderer = renderer;
    return me;
}

void pebbleWindow_free(PebbleWindow* me)
{
    if (me == NULL)
        return;
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
    SDL_Color* texPixels = me->textureData;

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
}

void pebbleWindow_onKeyDown(Window* window, SDL_Keysym sym, void* userdata)
{
    UNUSED(window);
    PebbleWindow* me = (PebbleWindow*)userdata;
    switch(sym.sym)
    {
        case (SDLK_w): renderer_move(me->renderer, xz_forward); break;
        case (SDLK_s): renderer_move(me->renderer, xz_backward); break;
        case (SDLK_a): renderer_move(me->renderer, xz_left); break;
        case (SDLK_d): renderer_move(me->renderer, xz_right); break;
        case (SDLK_UP): renderer_moveVertical(me->renderer, xy_up); break;
        case (SDLK_DOWN): renderer_moveVertical(me->renderer, xy_down); break;
        case (SDLK_LEFT): renderer_rotate(me->renderer, rotationLeft); break;
        case (SDLK_RIGHT): renderer_rotate(me->renderer, rotationRight); break;
        case(SDLK_SPACE):
        {
            Location playerLocation;
            playerLocation.sector = 0;
            playerLocation.angle = real_degToRad(real_from_int(0));
            playerLocation.height = real_zero;
            playerLocation.position = xz(real_from_int(20), real_from_int(20));

            renderer_moveTo(me->renderer, playerLocation);
        }break;
    }
}

GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window)
{
    return safeFramebuffer_getScreenBuffer(window->framebuffer);
}

ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* me)
{
    return me->window;
}
