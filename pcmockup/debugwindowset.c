#include "pcmockup.h"

#define WINDOW_GAP 64

struct DebugWindowSet
{
    int count;
    DebugWindow** windows;
    Renderer* renderer;
};

DebugWindowSet* debugWindowSet_init(SDL_Rect mainWindowBounds, Renderer* renderer)
{
    DebugWindowSet* me = (DebugWindowSet*)malloc(sizeof(DebugWindowSet));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(DebugWindowSet));
    me->renderer = renderer;

#ifdef DEBUG_WINDOWS
    me->count = renderer_getDebugCount(renderer);
    me->windows = (DebugWindow**)malloc(sizeof(DebugWindow*) * me->count);
    if (me->windows == NULL)
    {
        fprintf(stderr, "Could not allocate %d debug windows\n", me->count);
        debugWindowSet_free(me);
        return NULL;
    }
    memset(me->windows, 0, sizeof(DebugWindow*) * me->count);

    SDL_Rect curBounds = mainWindowBounds;
    curBounds.x += mainWindowBounds.w + WINDOW_GAP;

    for (int i = 0; i < me->count; i++)
    {
        me->windows[i] = debugWindow_init(
            curBounds, i,
            renderer_getDebugName(renderer, i)
        );
        if (me->windows[i] == NULL)
        {
            debugWindowSet_free(me);
            return NULL;
        }

        curBounds.y += mainWindowBounds.h + WINDOW_GAP;
    }
#endif

    return me;
}

void debugWindowSet_free(DebugWindowSet* me)
{
    if (me == NULL)
        return;
    if (me->windows != NULL)
    {
        for (int i = 0; i < me->count; i++)
        {
            if (me->windows[i] != NULL)
                debugWindow_free(me->windows[i]);
        }
        free(me->windows);
    }
    free(me);
    return;
}

void debugWindowSet_update(DebugWindowSet* me)
{
#ifdef DEBUG_WINDOWS
    for (int i = 0; i < me->count; i++)
    {
        debugWindow_startUpdate(me->windows[i]);
        const DebugInfo* info = debugWindow_getDebugInfo(me->windows[i]);
        renderer_renderDebug(me->renderer, info);
        debugWindow_endUpdate(me->windows[i]);
    }
#endif
}

void debugWindowSet_handleUpdate(DebugWindowSet* me, const SDL_Event* ev)
{
    for (int i = 0; i < me->count; i++)
        debugWindow_handleEvent(me->windows[i], ev);
}
