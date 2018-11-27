#include "pcmockup.h"
#include "renderer.h"
#include "platform.h"

#include <string.h>

struct DebugWindow
{
    ImageWindow* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    Renderer* podRenderer;
    const DebugView* view;
    xz_t position, offset;
    real_t zoom;
};

SDL_Surface* createSDLSurface(int w, int h, Uint32 format)
{
    SDL_PixelFormat* formatInfo = SDL_AllocFormat(format);
    if (formatInfo == NULL)
        return NULL;
    SDL_Surface* surface = SDL_CreateRGBSurface(
        SDL_SWSURFACE, w, h, formatInfo->BitsPerPixel,
        formatInfo->Rmask, formatInfo->Gmask, formatInfo->Bmask, formatInfo->Amask
    );
    SDL_FreeFormat(formatInfo);
    return surface;
}

void debugWindow_onDrag(Window* window, int button, ImVec2 delta, void* userdata);

DebugWindow* debugWindow_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* view, Renderer* podRenderer)
{
    DebugWindow* me = (DebugWindow*)malloc(sizeof(DebugWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(DebugWindow));

    GRect b = { { bounds.x, bounds.y }, { bounds.w, bounds.h} };
    me->window = imageWindow_init(parent, view->name, b, false);
    if (me->window == NULL)
    {
        debugWindow_free(me);
        return NULL;
    }
    window_setDragCallback(imageWindow_asWindow(me->window), debugWindow_onDrag, me);

    me->surface = createSDLSurface(bounds.w, bounds.h, SDL_PIXELFORMAT_ABGR8888);
    if (me->surface == NULL)
    {
        fprintf(stderr, "createSDLSurface: %s\n", SDL_GetError());
        debugWindow_free(me);
        return NULL;
    }

    me->renderer = SDL_CreateSoftwareRenderer(me->surface);
    if (me->renderer == NULL)
    {
        debugWindow_free(me);
        return NULL;
    }

    me->position = xz(real_zero, real_zero);
    me->zoom = real_one;
    me->offset = xz(real_from_int(bounds.w / 2), real_from_int(bounds.h / 2));
    me->podRenderer = podRenderer;
    me->view = view;
    return me;
}

void debugWindow_free(DebugWindow* me)
{
    if (me == NULL)
        return;
    if (me->window != NULL)
        imageWindow_free(me->window);
    if (me->surface != NULL)
        SDL_FreeSurface(me->surface);
    if (me->renderer != NULL)
        SDL_DestroyRenderer(me->renderer);
    free(me);
}

void debugWindow_startUpdate(DebugWindow* me)
{
    float scale = real_to_float(me->zoom);
    SDL_SetRenderDrawColor(me->renderer, 0, 0, 0, 255);
    SDL_RenderClear(me->renderer);
    SDL_RenderSetScale(me->renderer, scale, scale);
}

void debugWindow_endUpdate(DebugWindow* me)
{
    imageWindow_setImageData(me->window, GSize(me->surface->w, me->surface->h), (SDL_Color*)me->surface->pixels);
}

void debugWindow_update(DebugWindow* me)
{
    debugWindow_startUpdate(me);
    me->view->callback.sdl(me->podRenderer, me->renderer, me->offset, me->view->userdata);
    debugWindow_endUpdate(me);
}

void debugWindow_onDrag(Window* window, int button, ImVec2 delta, void* userdata)
{
    UNUSED(window);
    if (button != 0)
        return;
    DebugWindow* me = (DebugWindow*)userdata;
    xz_t move = xz_invScale((xz_t) { real_from_int(delta.x), real_from_int(delta.y) }, me->zoom);
    me->position = xz_sub(me->position, move);

    xz_t halfSize = xz(real_from_int(me->surface->w / 2), real_from_int(me->surface->h / 2));
    me->offset = xz_sub(
        xz_invScale(halfSize, me->zoom),
        me->position
    );
}

const DebugView* debugWindow_getDebugView(const DebugWindow* me)
{
    return me->view;
}

ImageWindow* debugWindow_asImageWindow(DebugWindow* me)
{
    return me->window;
}
