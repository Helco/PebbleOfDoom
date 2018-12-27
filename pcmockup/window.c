#define _CRT_NONSTDC_NO_DEPRECATE
#include "window_internal.h"
#include <assert.h>

#define MOUSE_BUTTON_COUNT 5 // even imgui uses this "magic" number
#define DEFAULT_MOUSE_THRESHOLD -1.0f
#define MAX_CALLBACKS 8

struct Window
{
    WindowContainer* parent;
    char* title;
    ImVec2 currentPos, currentSize;
    ImVec2 initialPos, initialSize;
    bool isFocused;
    ImGuiWindowFlags flags;
    WindowOpenState openState;
    ImVec2 lastDragDelta[MOUSE_BUTTON_COUNT];

    WindowCallbacks callbacks[MAX_CALLBACKS];
    int callbackCount;
    bool isDestructing;
};

void window_callDestructor(Window* me);
void window_callBeforeUpdate(Window* me);
void window_callContentUpdate(Window* me);
void window_callAfterUpdate(Window* me);
void window_callMainMenubar(void* userdata);
void window_callDrag(Window* me, int mouseKey, ImVec2 delta);
void window_callKeyDown(Window* me, SDL_Keysym sym);
void window_callKeyUp(Window* me, SDL_Keysym sym);

Window* window_init(WindowContainer* parent)
{
    Window* me = (Window*)malloc(sizeof(Window));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(Window));

    me->parent = parent;
    me->initialPos = me->currentPos = (ImVec2) { -1, -1 };
    me->initialSize = me->currentSize = (ImVec2) { -1, -1 };
    window_setTitle(me, "");
    windowContainer_addMenubarHandlerWithWindow(me->parent,
        window_callMainMenubar, me, NULL, me);
    return me;
}

void window_free(Window* me)
{
    windowContainer_freeWindow(me->parent, me);
}

void window_internal_free(Window* me)
{
    if (me == NULL || me->isDestructing)
        return;
    me->isDestructing = true;
    window_callDestructor(me);
    if (me->title != NULL)
        free(me->title);
    free(me);
}

void window_update(Window* me)
{
    if (me->openState == WindowOpenState_Closed)
        return;
    window_callBeforeUpdate(me);

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

    window_callContentUpdate(me);

    igEnd();
    if (isOpenPtr != NULL)
        me->openState = isOpen ? WindowOpenState_Open : WindowOpenState_Closed;

    window_callAfterUpdate(me);
}

static bool window_shouldHandleDrag(const Window* me, int button)
{
    if (!igIsMouseDragging(button, DEFAULT_MOUSE_THRESHOLD))
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
        window_callDrag(me, button, deltaDelta);
        me->lastDragDelta[button] = delta;
    }
}

void window_handleKeyEvent(Window* me, SDL_Keysym sym, bool isDown)
{
    if (isDown)
        window_callKeyDown(me, sym);
    else
        window_callKeyUp(me, sym);
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

const char* window_getMenubarSection(const Window* me)
{
    return *windowContainer_getMenubarSectionPtr(me->parent, me);
}

bool window_hasTag(const Window* window, uint32_t tag)
{
    for (int i = 0; i < window->callbackCount; i++) {
        if (window->callbacks[i].tag == tag)
            return true;
    }
    return false;
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

void window_updateMenubar(Window* me)
{
    bool isOpen = (me->openState == WindowOpenState_Open);
    bool* isOpenPtr = me->openState == WindowOpenState_Unclosable ? NULL : &isOpen;
    igMenuItemBoolPtr(me->title, NULL, isOpenPtr, true);
    if (isOpenPtr)
        window_setOpenState(me, isOpen ? WindowOpenState_Open : WindowOpenState_Closed);
}

void window_setMenubarSection(Window* me, const char* section)
{
    char** sectionPtr = windowContainer_getMenubarSectionPtr(me->parent, me);
    if (*sectionPtr != NULL)
        free(*sectionPtr);
    *sectionPtr = (section != NULL && strcmp(section, "") != 0)
        ? strdup(section)
        : NULL;
}

void window_addCallbacks(Window* me, WindowCallbacks callbacks)
{
    assert(me->callbackCount < MAX_CALLBACKS);
    me->callbacks[me->callbackCount] = callbacks;
    me->callbackCount++;
}

void window_callDestructor(Window* me)
{
    // call destructors in reverse for more predictable
    for (int i = me->callbackCount - 1; i >= 0; i--) {
        if (me->callbacks[i].destruct)
            me->callbacks[i].destruct(me->callbacks[i].userdata);
    }
}

void window_callBeforeUpdate(Window* me)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].beforeUpdate)
            me->callbacks[i].beforeUpdate(me->callbacks[i].userdata);
    }
}

void window_callContentUpdate(Window* me)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].contentUpdate)
            me->callbacks[i].contentUpdate(me->callbacks[i].userdata);
    }
}

void window_callAfterUpdate(Window* me)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].afterUpdate)
            me->callbacks[i].afterUpdate(me->callbacks[i].userdata);
    }
}

void window_callMainMenubar(void* userdata)
{
    Window* me = (Window*)userdata;
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].mainMenubar)
            me->callbacks[i].mainMenubar(me->callbacks[i].userdata);
    }
}

void window_callDrag(Window* me, int mouseKey, ImVec2 delta)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].drag)
            me->callbacks[i].drag(mouseKey, delta, me->callbacks[i].userdata);
    }
}

void window_callKeyDown(Window* me, SDL_Keysym sym)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].keyDown)
            me->callbacks[i].keyDown(sym, me->callbacks[i].userdata);
    }
}

void window_callKeyUp(Window* me, SDL_Keysym sym)
{
    for (int i = 0; i < me->callbackCount; i++) {
        if (me->callbacks[i].keyUp)
            me->callbacks[i].keyUp(sym, me->callbacks[i].userdata);
    }
}
