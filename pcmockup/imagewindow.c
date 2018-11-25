#define _CRT_NONSTDC_NO_DEPRECATE
#include "pcmockup.h"
#include <glad/glad.h>
#include <cimgui.h>
#include <float.h>

struct ImageWindow
{
    char* title;
    GLuint textureID;
    GSize windowSize, imageSize;
    float aspect;
    SDL_Rect lastContentPos;
    bool isOpen, isEssential;
    ImVec2 initialPosition;
    float toolbarHeight;
};

const Uint32 imageWindow_SDLPixelFormat = SDL_PIXELFORMAT_ABGR8888;

ImageWindow* imageWindow_init(const char* title, GSize initialSize, bool_t isEssential)
{
    ImageWindow* me = (ImageWindow*)malloc(sizeof(ImageWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(ImageWindow));

    me->title = strdup(title);
    if (me->title == NULL)
    {
        fprintf(stderr, "strdup: failure\n");
        imageWindow_free(me);
        return NULL;
    }

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

    me->isOpen = true;
    me->isEssential = isEssential;
    me->windowSize = initialSize;
    me->aspect = (float)initialSize.w / initialSize.h;
    me->initialPosition.x = -1; // undefined initial position
    return me;
}

void imageWindow_free(ImageWindow* me)
{
    if (me == NULL)
        return;
    if (me->textureID != 0)
        glDeleteTextures(1, &me->textureID);
    if (me->title != NULL)
        free(me->title);
    free(me);
}

void imageWindow_setImageData(ImageWindow* me, GSize size, const SDL_Color* data)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, me->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    me->imageSize = size;
    me->aspect = (float)size.w / (float)size.h;
    me->windowSize.w = max(me->windowSize.w, size.w);
    me->windowSize.h = max(me->windowSize.h, size.h);
}

SDL_Rect imageWindow_getContentPos(ImageWindow* me)
{
    return me->lastContentPos;
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

void imageWindow_update(ImageWindow* me)
{
    if (!me->isOpen)
        return;
    const ImVec2
        size = { (float)me->windowSize.w, (float)me->windowSize.h },
        minSize = { (float)me->imageSize.w, (float)me->imageSize.h },
        maxSize = { FLT_MAX, FLT_MAX },
        zero = { 0, 0 },
        one = { 1 + FLT_EPSILON, 1 + FLT_EPSILON };
    ImVec2 toolbarSize, windowPos;
    const ImVec4
        tintColor = { 1, 1, 1, 1 },
        borderColor = { 0, 0, 0, 0 };
    bool* isOpenPtr = me->isEssential
        ? NULL // essential windows don't get a close button
        : &me->isOpen;

    if (me->initialPosition.x >= 0 && me->initialPosition.y >= 0)
        igSetNextWindowPos(me->initialPosition, ImGuiCond_Once, zero);
    igSetNextWindowSizeConstraints(minSize, maxSize, imageWindow_constrainWindowSize, me);
    igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, zero);  // space between image and window border
    igBegin(me->title, isOpenPtr,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse);
    igGetItemRectSize_nonUDT(&toolbarSize);
    igGetWindowPos_nonUDT(&windowPos);
    igImageButton((ImTextureID)(intptr_t)me->textureID, size, zero, one, 0, borderColor, tintColor);
    igPopStyleVar(1);

    me->windowSize.w = (int16_t)igGetWindowWidth();
    me->windowSize.h = (int16_t)(igGetWindowHeight() - toolbarSize.y);
    me->toolbarHeight = toolbarSize.y;
    igEnd();

    me->lastContentPos = (SDL_Rect) {
        (int)(windowPos.x),
        (int)(windowPos.y),
        (int)me->windowSize.w,
        (int)me->windowSize.h
    };
}

void imageWindow_setInitialPosition(ImageWindow* me, GPoint initialPosition)
{
    me->initialPosition = (ImVec2) {
        (float)initialPosition.x,
        (float)initialPosition.y
    };
}

void imageWindow_toggle(ImageWindow* me, bool_t isOpen)
{
    me->isOpen = isOpen;
}

bool_t imageWindow_isOpen(ImageWindow* me)
{
    return me->isOpen;
}
