#include "pcmockup.h"
#include "renderer.h"
#include "platform.h"

#include <string.h>

struct DebugWindowSDL
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

void debugWindowSDL_free(void *userdata);
void debugWindowSDL_contentUpdate(void* userdata);
void debugWindowSDL_updateMenubar(void* userdata);
void debugWindowSDL_onDrag(int button, ImVec2 delta, void* userdata);
void debugWindowSDL_onKeyDown(SDL_Keysym sym, void* userdata);

DebugWindowSDL* debugWindowSDL_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* view, Renderer* podRenderer)
{
    DebugWindowSDL* me = (DebugWindowSDL*)malloc(sizeof(DebugWindowSDL));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(DebugWindowSDL));

    GRect b = { { bounds.x, bounds.y }, { bounds.w, bounds.h} };
    me->window = imageWindow_init(parent, view->name, b, false);
    if (me->window == NULL)
    {
        debugWindowSDL_free(me);
        return NULL;
    }
    window_setMenubarSection(imageWindow_asWindow(me->window), "Debug windows");
    window_setOpenState(imageWindow_asWindow(me->window), view->startsOpened
        ? WindowOpenState_Open : WindowOpenState_Closed);
    window_addCallbacks(imageWindow_asWindow(me->window), (WindowCallbacks) {
        .destruct = debugWindowSDL_free,
        .drag = debugWindowSDL_onDrag,
        .keyDown = debugWindowSDL_onKeyDown,
        .contentUpdate = debugWindowSDL_contentUpdate,
        .mainMenubar = debugWindowSDL_updateMenubar,
        .userdata = me
    });

    me->surface = createSDLSurface(bounds.w, bounds.h, SDL_PIXELFORMAT_ABGR8888);
    if (me->surface == NULL)
    {
        fprintf(stderr, "createSDLSurface: %s\n", SDL_GetError());
        debugWindowSDL_free(me);
        return NULL;
    }

    me->renderer = SDL_CreateSoftwareRenderer(me->surface);
    if (me->renderer == NULL)
    {
        debugWindowSDL_free(me);
        return NULL;
    }

    me->position = xz(real_zero, real_zero);
    me->zoom = real_one;
    me->offset = xz(real_from_int(bounds.w / 2), real_from_int(bounds.h / 2));
    me->podRenderer = podRenderer;
    me->view = view;
    return me;
}

void debugWindowSDL_free(void *userdata)
{
    DebugWindowSDL* me = (DebugWindowSDL*)userdata;
    if (me->window != NULL)
        window_scheduleFree(imageWindow_asWindow(me->window));
    if (me->surface != NULL)
        SDL_FreeSurface(me->surface);
    if (me->renderer != NULL)
        SDL_DestroyRenderer(me->renderer);
    free(me);
}

void debugWindowSDL_contentUpdate(void* userdata)
{
    DebugWindowSDL* me = (DebugWindowSDL*)userdata;
    float scale = real_to_float(me->zoom);
    SDL_SetRenderDrawColor(me->renderer, 0, 0, 0, 255);
    SDL_RenderClear(me->renderer);
    SDL_RenderSetScale(me->renderer, scale, scale);

    me->view->callback.sdl(me->podRenderer, me->renderer, me->offset, me->view->userdata);

    imageWindow_setImageData(me->window, GSize(me->surface->w, me->surface->h), (SDL_Color*)me->surface->pixels);
}

void debugWindowSDL_updateMenubar(void* userdata)
{
    DebugWindowSDL* me = (DebugWindowSDL*)userdata;
    bool isOpen = imageWindow_isOpen(me->window);
    igMenuItemBoolPtr(me->view->name, NULL, &isOpen, true);
    imageWindow_toggle(me->window, isOpen);
}

void debugWindowSDL_updateOffset(DebugWindowSDL* me)
{
    xz_t halfSize = xz(real_from_int(me->surface->w / 2), real_from_int(me->surface->h / 2));
    me->offset = xz_sub(
        xz_invScale(halfSize, me->zoom),
        me->position
    );
}

void debugWindowSDL_onDrag(int button, ImVec2 delta, void* userdata)
{
    if (button != 2) // middle mouse button
        return;
    DebugWindowSDL* me = (DebugWindowSDL*)userdata;
    xz_t move = xz_invScale((xz_t) { real_from_int(delta.x), real_from_int(delta.y) }, me->zoom);
    me->position = xz_sub(me->position, move);
    debugWindowSDL_updateOffset(me);
}

void debugWindowSDL_onKeyDown(SDL_Keysym sym, void* userdata)
{
    static const float zoomSpeed = 0.1f;
    static const float moveSpeed = 8.0f;
    real_t zoomDelta = real_zero;
    xz_t moveDelta = xz_zero;
    switch (sym.sym)
    {
        case (SDLK_PLUS):
        case (SDLK_KP_PLUS): zoomDelta = real_from_float(zoomSpeed); break;
        case (SDLK_MINUS):
        case (SDLK_KP_MINUS): zoomDelta = real_from_float(-zoomSpeed); break;
        case (SDLK_w):
        case (SDLK_UP): moveDelta = xz(real_zero, real_from_float(-moveSpeed)); break;
        case (SDLK_s):
        case (SDLK_DOWN): moveDelta = xz(real_zero, real_from_float(moveSpeed)); break;
        case (SDLK_a):
        case (SDLK_LEFT): moveDelta = xz(real_from_float(-moveSpeed), real_zero); break;
        case (SDLK_d):
        case (SDLK_RIGHT): moveDelta = xz(real_from_float(moveSpeed), real_zero); break;
        default: return;
    }
    DebugWindowSDL* me = (DebugWindowSDL*)userdata;
    me->position = xz_sub(me->position, xz_invScale(moveDelta, me->zoom));
    me->zoom = real_add(me->zoom, real_mul(me->zoom, zoomDelta));
    debugWindowSDL_updateOffset(me);
}

const DebugView* debugWindowSDL_getDebugView(const DebugWindowSDL* me)
{
    return me->view;
}

ImageWindow* debugWindowSDL_asImageWindow(DebugWindowSDL* me)
{
    return me->window;
}
