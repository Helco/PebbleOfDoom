#include "pcmockup.h"
#include "platform.h"
#include "segame.h"
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
    RendererColorFormat format;
    SEGame game;
    int startSelectDown;
};

void pebbleWindow_free(void* userdata);
void pebbleWindow_contentUpdate(void* userdata);
void pebbleWindow_onKeyDown(SDL_Keysym sym, void* userdata);
void pebbleWindow_onKeyUp(SDL_Keysym sym, void* userdata);

PebbleWindow* pebbleWindow_init(WindowContainer* parent, GRect initialBounds, GSize pebbleSize, RendererColorFormat format, Renderer* renderer, LevelManagerHandle levelManager)
{
    PebbleWindow* me = (PebbleWindow*)malloc(sizeof(PebbleWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(PebbleWindow));

    me->window = imageWindow_init(parent, "Pebble screen", initialBounds, true);
    if (me->window == NULL)
    {
        pebbleWindow_free(me);
        return NULL;
    }
    window_addCallbacks(imageWindow_asWindow(me->window), (WindowCallbacks) {
        .tag = PebbleWindow_Tag,
        .destruct = pebbleWindow_free,
        .contentUpdate = pebbleWindow_contentUpdate,
        .keyDown = pebbleWindow_onKeyDown,
        .keyUp = pebbleWindow_onKeyUp,
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

    me->framebuffer = safeFramebuffer_init(pebbleSize, format, CANARY_BUFFER_SIZE);
    if (me->framebuffer == NULL)
    {
        pebbleWindow_free(me);
        return NULL;
    }

    segame_init(&me->game, renderer, levelManager);

    me->pebbleSize = pebbleSize;
    me->format = format;
    me->renderer = renderer;
    return me;
}

void pebbleWindow_free(void* userdata)
{
    PebbleWindow* me = (PebbleWindow*)userdata;
    if (me->window != NULL)
        window_free(imageWindow_asWindow(me->window));
    if (me->textureData != NULL)
        free(me->textureData);
    if (me->framebuffer != NULL)
        safeFramebuffer_free(me->framebuffer);
    if (me->texturePixelFormat != NULL)
        SDL_FreeFormat(me->texturePixelFormat);
    segame_free(&me->game);
    free(me);
}

void* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window)
{
    return safeFramebuffer_getScreenBuffer(window->framebuffer);
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
    SDL_Color* texPixels = me->textureData;
    uint32_t* itTexPixel;

    if (me->format == RendererColorFormat_8BitColor) {
        const GColor* pebblePixels = (const GColor*)pebbleWindow_getPebbleFramebuffer(me);
        const GColor* itPebblePixel;
        for (int y = 0; y < me->pebbleSize.h; y++)
        {
            itTexPixel = (uint32_t*)texPixels;
            for (int x = 0; x < me->pebbleSize.w; x++)
            {
                itPebblePixel = pebblePixels + x * me->pebbleSize.h + (me->pebbleSize.h - y - 1);
                SDL_Color color = prv_convertGColorTo32Bit(*itPebblePixel);
                *itTexPixel = SDL_MapRGBA(me->texturePixelFormat,
                    color.r, color.g, color.b, color.a);

                itTexPixel++;
            }

            // Advance to next line
            texPixels += me->pebbleSize.w;
        }
    }
    else if (me->format == RendererColorFormat_1BitBW) {
        const uint8_t* pebblePixels = (const uint8_t*)pebbleWindow_getPebbleFramebuffer(me);
        const int stride = rendererColorFormat_getStride(me->format);
        for (int y = 0; y < me->pebbleSize.h; y++)
        {
            itTexPixel = (uint32_t*)texPixels;
            for (int x = 0; x < me->pebbleSize.w; x++)
            {
                int pebbleY = (me->pebbleSize.h - y - 1);
                const uint8_t* itPebblePixel = pebblePixels + x * stride + pebbleY / 8;
                const bool bit = *itPebblePixel & (1 << (pebbleY % 8));
                *itTexPixel = SDL_MapRGBA(me->texturePixelFormat,
                    bit * 255, bit * 255, bit * 255, 255);

                itTexPixel++;
            }

            // Advance to next line
            texPixels += me->pebbleSize.w;
        }
    }
    else
        assert(false && "Unknown RendererColorFormat for PebbleWindow");
}

static uint8_t fauxFramebuffer[20 * 168];
#define W { .r = 3, .g = 3, .b = 3, .a = 3 }
#define B { .r = 0, .g = 0, .b = 0, .a = 3 }

void convertFauxFramebuffer(uint8_t* target)
{
    static union {
        uint32_t pixelQuads[16];
        GColor pixelColors[16 * 4];
    } l = {
        .pixelColors = {
            B, B, B, B,
            W, B, B, B,
            B, W, B, B,
            W, W, B, B,
            B, B, W, B,
            W, B, W, B,
            B, W, W, B,
            W, W, W, B,
            B, B, B, W,
            W, B, B, W,
            B, W, B, W,
            W, W, B, W,
            B, B, W, W,
            W, B, W, W,
            B, W, W, W,
            W, W, W, W,
        }
    };

    uint8_t* source = fauxFramebuffer;
    for (int i = 0; i < 168; i++)
    {
        for (int j = 0; j < 144; j += 8)
        {
            uint8_t s = source[j / 8];
            *((uint32_t*)&target[j + 0]) = l.pixelQuads[s & 0xf];
            *((uint32_t*)&target[j + 4]) = l.pixelQuads[s >> 4];
        }
        source += 20;
        target += 144;
    }
}

void pebbleWindow_contentUpdate(void* userdata)
{
    PebbleWindow* me = (PebbleWindow*)userdata;

    if (me->startSelectDown > 0 && SDL_GetTicks() - me->startSelectDown > 500)
    {
        me->startSelectDown = -1;
        segame_input_select_long_click(&me->game);
    }

    segame_update(&me->game);

    safeFramebuffer_prepare(me->framebuffer);
    RendererTarget target = {
        .framebuffer = me->format == RendererColorFormat_1BitBW
            ? pebbleWindow_getPebbleFramebuffer(me)
            : fauxFramebuffer,
        .colorFormat = RendererColorFormat_1BitBW
    };
    segame_render(&me->game, target);
    if (me->format != RendererColorFormat_1BitBW)
        convertFauxFramebuffer((uint8_t*)pebbleWindow_getPebbleFramebuffer(me));
    safeFramebuffer_check(me->framebuffer);
    prv_pebbleWindow_convertPebbleToTexture(me);
    imageWindow_setImageData(me->window, me->pebbleSize, me->textureData);
}

void pebbleWindow_onKeyDown(SDL_Keysym sym, void* userdata)
{
    const int maxStepHeight = 5;
    PebbleWindow* me = (PebbleWindow*)userdata;
    switch(sym.sym)
    {
    case(SDLK_i): if (me->startSelectDown == 0) me->startSelectDown = SDL_GetTicks(); break;
    case (SDLK_u): segame_input_direction_raw(&me->game, false, true); break;
    case (SDLK_o): segame_input_direction_raw(&me->game, true, true); break;

        case (SDLK_w): renderer_walk(me->renderer, xz_forward, maxStepHeight); break;
        case (SDLK_s): renderer_walk(me->renderer, xz_backward, maxStepHeight); break;
        case (SDLK_a): renderer_walk(me->renderer, xz_left, maxStepHeight); break;
        case (SDLK_d): renderer_walk(me->renderer, xz_right, maxStepHeight); break;
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

void pebbleWindow_onKeyUp(SDL_Keysym sym, void* userdata)
{
    PebbleWindow* me = (PebbleWindow*)userdata;
    switch (sym.sym)
    {
    case (SDLK_u):
        segame_input_direction_raw(&me->game, false, false);
        segame_input_direction_click(&me->game, false);
        break;
    case (SDLK_o):
        segame_input_direction_raw(&me->game, true, false);
        segame_input_direction_click(&me->game, true);
        break;
    case (SDLK_i):
        if (me->startSelectDown > 0)
            segame_input_select_click(&me->game);
        me->startSelectDown = 0;
        break;
    case (SDLK_k):
        segame_input_back_click(&me->game);
        break;
    }
}

ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* me)
{
    return me->window;
}
