#include "pcmockup.h"
#include "renderer.h"
#include "platform.h"

struct DebugWindowImGui
{
    Window* window;
    Renderer* podRenderer;
    const DebugView* view;
    ImGuiWindowFlags flags;
};

void debugWindowImGui_free(void* userdata);
void debugWindowImGui_contentUpdate(void* userdata);
void debugWindowImGui_updateMenubar(void* userdata);

DebugWindowImGui* debugWindowImGui_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* view, Renderer* renderer)
{
    DebugWindowImGui* me = (DebugWindowImGui*)malloc(sizeof(DebugWindowImGui));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(DebugWindowImGui));
    me->flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize;

    GRect b = { { bounds.x, bounds.y }, { bounds.w, bounds.h} };
    me->window = windowContainer_newWindow(parent, view->name);
    if (me->window == NULL)
    {
        debugWindowImGui_free(me);
        return NULL;
    }
    window_setInitialBounds(me->window, b);
    window_setFlags(me->window, me->flags);
    window_setMenubarSection(me->window, "Debug windows");
    window_setOpenState(me->window, view->startsOpened
        ? WindowOpenState_Open : WindowOpenState_Closed);
    window_addCallbacks(me->window, (WindowCallbacks) {
        .destruct = debugWindowImGui_free,
        .contentUpdate = debugWindowImGui_contentUpdate,
        .mainMenubar = debugWindowImGui_updateMenubar,
        .userdata = me
    });

    me->view = view;
    me->podRenderer = renderer;
    return me;
}

void debugWindowImGui_free(void* userdata)
{
    DebugWindowImGui* me = (DebugWindowImGui*)userdata;
    if (me->window != NULL)
        window_scheduleFree(me->window);
    free(me);
}

void debugWindowImGui_contentUpdate(void* userdata)
{
    DebugWindowImGui* me = (DebugWindowImGui*)userdata;
    me->view->callback.imgui(me->podRenderer, &me->flags, me->view->userdata);
    window_setFlags(me->window, me->flags);
}

void debugWindowImGui_updateMenubar(void* userdata)
{
    DebugWindowImGui* me = (DebugWindowImGui*)userdata;
    bool isOpen = window_getOpenState(me->window) == WindowOpenState_Open;
    igMenuItemBoolPtr(me->view->name, NULL, &isOpen, true);
    window_setOpenState(me->window, isOpen
        ? WindowOpenState_Open
        : WindowOpenState_Closed);
}
