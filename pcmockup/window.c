#define _CRT_NONSTDC_NO_DEPRECATE
#include "window_internal.h"

#define MOUSE_BUTTON_COUNT 5 // even imgui uses this "magic" number
#define DEFAULT_MOUSE_THRESHOLD -1.0f

struct Window
{
    char* title;
    ImVec2 currentPos, currentSize;
    ImVec2 initialPos, initialSize;
    bool isFocused;
    ImGuiWindowFlags flags;
    WindowOpenState openState;
    ImVec2 lastDragDelta[MOUSE_BUTTON_COUNT];

    void* onDragUserdata;
    WindowUpdateCallbacks onUpdate;
    WindowDragCallback onDrag;
    WindowKeyCallbacks onKey;
};

Window* window_init()
{
    Window* me = (Window*)malloc(sizeof(Window));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(Window));

    me->initialPos = me->currentPos = (ImVec2) { -1, -1 };
    me->initialSize = me->currentSize = (ImVec2) { -1, -1 };
    window_setTitle(me, "");
    return me;
}

void window_free(Window* me)
{
    if (me == NULL)
        return;
    if (me->title != NULL)
        free(me->title);
    free(me);
}

void window_update(Window* me)
{
    if (me->openState == WindowOpenState_Closed)
        return;
    if (me->onUpdate.before != NULL)
        me->onUpdate.before(me, me->onUpdate.userdata);

    bool isOpen = (me->openState == WindowOpenState_Open);
    bool* isOpenPtr = me->openState == WindowOpenState_Unclosable ? NULL : &isOpen;
    if (me->initialPos.x >= 0 && me->initialPos.y >= 0)
        igSetNextWindowPos(me->initialPos, ImGuiCond_Once, (ImVec2) { 0, 0 });
    if (me->initialSize.x > 0 && me->initialSize.y > 0)
        igSetNextWindowSize(me->initialSize, ImGuiCond_Once);
    igBegin(me->title, isOpenPtr, me->flags);
    igGetCursorScreenPos_nonUDT(&me->currentPos); // handles toolbar height
    igGetWindowSize_nonUDT(&me->currentSize);
    me->isFocused = igIsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    if (me->onUpdate.content != NULL)
        me->onUpdate.content(me, me->onUpdate.userdata);

    igEnd();
    if (isOpenPtr != NULL)
        me->openState = isOpen ? WindowOpenState_Open : WindowOpenState_Closed;

    if (me->onUpdate.after != NULL)
        me->onUpdate.after(me, me->onUpdate.userdata);
}

static bool window_shouldHandleDrag(const Window* me, int button)
{
    if (!igIsMouseDragging(button, DEFAULT_MOUSE_THRESHOLD) || me->onDrag == NULL)
        return false;
    const ImVec2 dragSource = igGetIO()->MouseClickedPos[button];
    return
        dragSource.x >= me->currentPos.x && dragSource.x < me->currentPos.x + me->currentSize.x &&
        dragSource.y >= me->currentPos.y && dragSource.y < me->currentPos.y + me->currentSize.y;
}

void window_handleDragEvent(Window* me)
{
    for (int button = 0; button < MOUSE_BUTTON_COUNT; button++)
    {
        if (!window_shouldHandleDrag(me, button))
        {
            me->lastDragDelta[button] = (ImVec2) { 0, 0 };
            continue;
        }
        ImVec2 lastDelta = me->lastDragDelta[button], delta;
        igGetMouseDragDelta_nonUDT(&delta, button, DEFAULT_MOUSE_THRESHOLD);

        ImVec2 deltaDelta = { delta.x - lastDelta.x, delta.y - lastDelta.y };
        me->onDrag(me, button, deltaDelta, me->onDragUserdata);
        me->lastDragDelta[button] = delta;
    }
}

void window_handleKeyEvent(Window* me, SDL_Keysym sym, bool isDown)
{
    if (isDown && me->onKey.down != NULL)
        me->onKey.down(me, sym, me->onKey.userdata);
    if (!isDown && me->onKey.up != NULL)
        me->onKey.up(me, sym, me->onKey.userdata);
}

GRect window_getBounds(const Window* me)
{
    return (GRect) {
        (GPoint) { (int)me->currentPos.x, (int)me->currentPos.y },
        (GSize) { (int)me->currentSize.x, (int)me->currentSize.y }
    };
}

WindowOpenState window_getOpenState(const Window* me)
{
    return me->openState;
}

bool window_isFocused(const Window* me)
{
    return me->isFocused;
}

void window_setTitle(Window* me, const char* title)
{
    if (me->title != NULL)
        free(me->title);
    me->title = strdup(title);
}

void window_setFlags(Window* me, ImGuiWindowFlags flags)
{
    me->flags = flags;
}

void window_setOpenState(Window* me, WindowOpenState state)
{
    me->openState = state;
}

void window_setInitialBounds(Window* me, GRect bounds)
{
    me->initialPos = (ImVec2) { (float)bounds.origin.x, (float)bounds.origin.y };
    me->initialSize = (ImVec2) { (float)bounds.size.w, (float)bounds.size.h };
    if (me->initialPos.x < 0 && me->initialPos.y < 0)
        me->currentPos = me->initialPos;
    if (me->initialSize.x < 0 && me->initialSize.y < 0)
        me->currentSize = me->initialSize;
}

void window_setUpdateCallbacks(Window* me, WindowUpdateCallbacks callbacks)
{
    me->onUpdate = callbacks;
}

void window_setDragCallback(Window* me, WindowDragCallback callback, void* userdata)
{
    me->onDrag = callback;
    me->onDragUserdata = userdata;
}

void window_setKeyCallbacks(Window* me, WindowKeyCallbacks callbacks)
{
    me->onKey = callbacks;
}
