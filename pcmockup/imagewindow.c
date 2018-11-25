#define _CRT_NONSTDC_NO_DEPRECATE
#include "pcmockup.h"
#include <glad/glad.h>
#include <cimgui.h>

struct ImageWindow
{
    char* title;
    GLuint textureID;
    GSize windowSize;
    float aspect;
    SDL_Rect lastContentPos;
    bool isOpen, isEssential;
    ImVec2 initialPosition;
};

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
    me->windowSize.w = max(me->windowSize.w, size.w);
    me->windowSize.h = max(me->windowSize.h, size.h);
}

SDL_Rect imageWindow_getContentPos(ImageWindow* me)
{
    return me->lastContentPos;
}

void imageWindow_update(ImageWindow* me)
{
    if (!me->isOpen)
        return;
    const SDL_Rect windowRect = { 0, 0, me->windowSize.w, me->windowSize.h };
    const SDL_Rect imageRect = findBestFit(windowRect, me->aspect);
    const ImVec2
        offset = { (float)imageRect.x, (float)imageRect.y },
        size = { (float)imageRect.w, (float)imageRect.h },
        zero = { 0, 0 },
        uv1 = { 1, 1 };
    ImVec2 toolbarSize, windowPos;
    const ImVec4
        tintColor = { 1, 1, 1, 1 },
        borderColor = { 0, 0, 0, 0 };
    bool* isOpenPtr = me->isEssential
        ? NULL // essential windows don't get a close button
        : &me->isOpen;

    if (me->initialPosition.x >= 0 && me->initialPosition.y >= 0)
        igSetNextWindowPos(me->initialPosition, ImGuiCond_Once, zero);
    igSetNextWindowContentSize(size);
    igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, zero);  // space between image and window border
    igPushStyleVarVec2(ImGuiStyleVar_ItemSpacing, zero);    // space between vertical centering dummy and image
    igPushStyleVarFloat(ImGuiStyleVar_IndentSpacing, 0.0f); // space between horizontal centering indent and image
    igBegin(me->title, isOpenPtr,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse);
    igGetItemRectSize_nonUDT(&toolbarSize);
    igGetWindowPos_nonUDT(&windowPos);
    igDummy(offset);    // center vertically
    igIndent(offset.x); // center horizontally
    igImage((ImTextureID)(intptr_t)me->textureID, size, zero, uv1, tintColor, borderColor);
    igPopStyleVar(3);

    me->windowSize.w = (int16_t)igGetWindowWidth();
    me->windowSize.h = (int16_t)(igGetWindowHeight() - toolbarSize.y);
    igEnd();

    me->lastContentPos = (SDL_Rect) {
        (int)(windowPos.x + imageRect.x),
        (int)(windowPos.y + toolbarSize.y + imageRect.y),
        (int)imageRect.w,
        (int)imageRect.h
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
