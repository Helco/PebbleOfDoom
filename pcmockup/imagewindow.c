#include "pcmockup.h"
#include <glad/glad.h>
#include "cimgui.include.h"
#include <float.h>
#include "platform.h"

struct ImageWindow
{
    Window* window;
    GLuint textureID;
    GSize imageSize;
    float aspect;
    bool isEssential;
    float toolbarHeight;
};

const Uint32 imageWindow_SDLPixelFormat = SDL_PIXELFORMAT_ABGR8888;

void imageWindow_beforeUpdate(Window* me, void* userdata);
void imageWindow_contentUpdate(Window* me, void* userdata);

ImageWindow* imageWindow_init(WindowContainer* parent, const char* title, GRect initialBounds, bool_t isEssential)
{
    ImageWindow* me = (ImageWindow*)malloc(sizeof(ImageWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(ImageWindow));

    me->window = windowContainer_newWindow(parent, title);
    if (me->window == NULL)
    {
        fprintf(stderr, "Could not allocate new window\n");
        imageWindow_free(me);
        return NULL;
    }
    window_setInitialBounds(me->window, initialBounds);
    window_setOpenState(me->window, isEssential ? WindowOpenState_Unclosable : WindowOpenState_Open);
    window_setFlags(me->window,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse);
    window_setUpdateCallbacks(me->window, (WindowUpdateCallbacks) {
        .before = imageWindow_beforeUpdate,
        .content = imageWindow_contentUpdate,
        .userdata = me
    });

    glGenTextures(1, &me->textureID);
    if (me->textureID == 0)
    {
        fprintf(stderr, "glGenTextures: %d\n", glGetError());
        imageWindow_free(me);
        return NULL;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, me->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    const SDL_Color black = { 255, 255, 255, 255 };
    imageWindow_setImageData(me, GSize(1, 1), &black);

    me->isEssential = isEssential;
    me->aspect = (float)initialBounds.size.w / initialBounds.size.h;
    return me;
}

void imageWindow_free(ImageWindow* me)
{
    if (me == NULL)
        return;
    if (me->textureID != 0)
        glDeleteTextures(1, &me->textureID);
    free(me);
}

void imageWindow_setImageData(ImageWindow* me, GSize size, const SDL_Color* data)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, me->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    me->imageSize = size;
    me->aspect = (float)size.w / (float)size.h;
}

void imageWindow_constrainWindowSize(ImGuiSizeCallbackData* data)
{
    ImageWindow* me = (ImageWindow*)data->UserData;
    data->DesiredSize.y -= me->toolbarHeight;

    ImVec2 byWidth = {
        floorf(data->DesiredSize.x),
        floorf(data->DesiredSize.x / me->aspect + me->toolbarHeight)
    };
    float byWidthArea = byWidth.x * byWidth.y;
    ImVec2 byHeight = {
        floorf(data->DesiredSize.y * me->aspect),
        floorf(data->DesiredSize.y + me->toolbarHeight)
    };
    float byHeightArea = byHeight.x * byHeight.y;

    data->DesiredSize = byWidthArea > byHeightArea ? byWidth : byHeight;
}

void imageWindow_beforeUpdate(Window* window, void* userdata)
{
    UNUSED(window);
    ImageWindow* me = (ImageWindow*)userdata;
    const ImVec2
        zero = { 0, 0 },
        minSize = { (float)me->imageSize.w, (float)me->imageSize.h },
        maxSize = { FLT_MAX, FLT_MAX };
    igSetNextWindowSizeConstraints(minSize, maxSize, imageWindow_constrainWindowSize, me);
    igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, zero);  // space between image and window border
}

void imageWindow_contentUpdate(Window* window, void* userdata)
{
    ImageWindow* me = (ImageWindow*)userdata;
    const GSize windowSize = window_getBounds(window).size;
    const ImVec2
        zero = { 0, 0 },
        one = { 1, 1 },
        size = { (float)windowSize.w, (float)windowSize.h - me->toolbarHeight };
    const ImVec4
        tintColor = { 1, 1, 1, 1 },
        borderColor = { 0, 0, 0, 0 };
    ImVec2 toolbarSize;

    igGetItemRectSize_nonUDT(&toolbarSize);
    me->toolbarHeight = toolbarSize.y;
    igImageButton((ImTextureID)(intptr_t)me->textureID, size, zero, one, 0, borderColor, tintColor);
    igPopStyleVar(1);
}

void imageWindow_toggle(ImageWindow* me, bool_t isOpen)
{
    window_setOpenState(me->window, isOpen ? WindowOpenState_Open : WindowOpenState_Closed);
}

bool_t imageWindow_isOpen(ImageWindow* me)
{
    return window_getOpenState(me->window) == WindowOpenState_Open;
}

Window* imageWindow_asWindow(ImageWindow* me)
{
    return me->window;
}
