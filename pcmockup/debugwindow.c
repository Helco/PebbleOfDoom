#include "pcmockup.h"
#include "renderer.h"

#include <string.h>

struct DebugWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect texturePos;
    DebugInfo info;
    xz_t position;
    real_t zoom;
};

DebugWindow* debugWindow_init(SDL_Rect bounds, int index, const char* title)
{
    DebugWindow* me = (DebugWindow*)malloc(sizeof(DebugWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(DebugWindow));

    me->window = SDL_CreateWindow(title,
        bounds.x, bounds.y,
        bounds.w, bounds.h,
        SDL_WINDOW_RESIZABLE);
    if (me->window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        debugWindow_free(me);
        return NULL;
    }

    me->renderer = SDL_CreateRenderer(me->window, -1, SDL_RENDERER_TARGETTEXTURE);
    if (me->renderer == NULL)
    {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        debugWindow_free(me);
        return NULL;
    }

    me->position = xz(real_zero, real_zero);
    me->zoom = real_one;
    me->info.index = index;
    me->info.offset = xz(real_from_int(bounds.w / 2), real_from_int(bounds.h / 2));
    me->info.ren = me->renderer;
    return me;
}

void debugWindow_free(DebugWindow* me)
{
    if (me == NULL)
        return;
    if (me->texture != NULL)
        SDL_DestroyTexture(me->texture);
    if (me->renderer != NULL)
        SDL_DestroyRenderer(me->renderer);
    if (me->window != NULL)
        SDL_DestroyWindow(me->window);
    free(me);
}

void debugWindow_startUpdate(DebugWindow* me)
{
    SDL_Rect src;
    int textureW = -1, textureH = -1;
    SDL_GetWindowSize(me->window, &src.w, &src.h);
    if (me->texture != NULL)
        SDL_QueryTexture(me->texture, NULL, NULL, &textureW, &textureH);
    me->texturePos = findBestFit(src, 1.0f);

    if (me->texturePos.w != textureW || me->texturePos.h != textureH)
    {
        if (me->texture != NULL)
            SDL_DestroyTexture(me->texture);
        me->texture = SDL_CreateTexture(me->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            me->texturePos.w, me->texturePos.h);
        if (me->texture == NULL)
        {
            fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
            exit(-1);
        }
    }

    SDL_SetRenderTarget(me->renderer, me->texture);
    SDL_SetRenderDrawColor(me->renderer, 0, 0, 0, 255);
    SDL_RenderClear(me->renderer);

    float scale = real_to_float(me->zoom);
    SDL_RenderSetScale(me->renderer, scale, scale);
}

void debugWindow_endUpdate(DebugWindow* me)
{
    SDL_RenderSetScale(me->renderer, 1.0f, 1.0f);
    SDL_SetRenderTarget(me->renderer, NULL);
    SDL_SetRenderDrawColor(me->renderer, 255, 0, 255, 255);
    SDL_RenderClear(me->renderer);
    SDL_SetRenderDrawColor(me->renderer, 255, 255, 255, 255);
    SDL_RenderCopy(me->renderer, me->texture, NULL, &me->texturePos);
    SDL_RenderPresent(me->renderer);
}

void debugWindow_handleEvent(DebugWindow* me, const SDL_Event* ev)
{
    Uint32 windowID = SDL_GetWindowID(me->window);
    if (ev->type == SDL_MOUSEMOTION && ev->motion.windowID == windowID && (ev->motion.state & SDL_BUTTON_LMASK) > 0)
    {
        xz_t move = xz(real_from_int(ev->motion.xrel), real_from_int(ev->motion.yrel));
        move = xz_invScale(move, me->zoom);
        me->position = xz_sub(me->position, move);
    }
    if (ev->type == SDL_MOUSEWHEEL && ev->wheel.windowID == windowID)
    {
        real_t zoom = real_from_int(ev->wheel.y);
        zoom = real_mul(real_div(zoom, real_from_int(10)), me->zoom);
        me->zoom = real_add(me->zoom, zoom);
    }
    if (ev->type == SDL_KEYDOWN && ev->key.windowID == windowID && ev->key.keysym.sym == SDLK_r)
    {
        me->position = xz(real_zero, real_zero);
    }

    // update render offset
    int textureW, textureH;
    SDL_QueryTexture(me->texture, NULL, NULL, &textureW, &textureH);
    xz_t halfSize = xz(real_from_int(textureW / 2), real_from_int(textureH / 2));
    me->info.offset = xz_sub(
        xz_invScale(halfSize, me->zoom),
        me->position
    );
}

const DebugInfo* debugWindow_getDebugInfo(DebugWindow* me)
{
    return &me->info;
}
