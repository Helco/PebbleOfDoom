#include <stdio.h>
#include "cimgui.include.h"
#include "pcmockup.h"
#include "renderer.h"
#include "texgen/texgen.h"
#include "platform.h"

static const GSize WINDOW_START_SIZE = { 1024, 768 };
static const int MAX_FRAMERATE = 60;

struct PCMockup
{
    Renderer *renderer;
    Level* level;
    TextureManager* textureManager;
    PebbleWindow *pebbleWindow;
    DebugWindowSet *debugWindowSet;
    WindowContainer* windowContainer;
    bool_t isRunning;
};

PCMockup *pcmockup_init()
{
    PCMockup *me = (PCMockup *)malloc(sizeof(PCMockup));
    if (me == NULL)
    {
        fprintf(stderr, "Could not allocate PCMockup context\n");
        return NULL;
    }
    memset(me, 0, sizeof(PCMockup));

    me->renderer = renderer_init();
    if (me->renderer == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    me->level = level_load(0);
    if (me->level == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    me->textureManager = textureManager_init();
    if (me->textureManager == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }
    renderer_setLevel(me->renderer, me->level);
    renderer_setTextureManager(me->renderer, me->textureManager);
    if (textureManager_registerFile(me->textureManager, "xor64.png") == INVALID_TEXTURE_ID)
    {
        pcmockup_free(me);
        return NULL;
    }
    TexGenerationContext* texgenctx = textureManager_createGeneratedTexture(me->textureManager, TexGenerator_Bricks, 256);
    texgen_execute(texgenctx);
    texgen_setGenerator(texgenctx, TexGenerator_Rand);
    texgen_setSize(texgenctx, 64);
    texgen_execute(texgenctx);

    me->windowContainer = windowContainer_init(WINDOW_START_SIZE);
    if (me->windowContainer == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    WindowGrid windowGrid;
    windowGrid.windowCount = 1 + renderer_getDebugCount(me->renderer);
    windowGrid.totalSize = WINDOW_START_SIZE;

    me->pebbleWindow = pebbleWindow_init(
        me->windowContainer,
        windowGrid_getSingleBounds(&windowGrid, 0),
        GSize(RENDERER_WIDTH, RENDERER_HEIGHT),
        me->renderer
    );
    if (me->pebbleWindow == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    me->debugWindowSet = debugWindowSet_init(
        me->windowContainer,
        &windowGrid,
        me->renderer
    );
    if (me->debugWindowSet == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    TextureWindow* textureWindow = textureWindow_init(me->windowContainer, me->textureManager);
    if (textureWindow == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    me->isRunning = true;
    return me;
}

void pcmockup_free(PCMockup *me)
{
    if (me == NULL)
        return;
    if (me->windowContainer != NULL)
        windowContainer_free(me->windowContainer);
    if (me->debugWindowSet != NULL)
        debugWindowSet_free(me->debugWindowSet);
    if (me->level != NULL)
        level_free(me->level);
    if (me->textureManager != NULL)
        textureManager_free(me->textureManager);
    if (me->renderer != NULL)
        renderer_free(me->renderer);
    free(me);
}

void pcmockup_updateMainMenubar(PCMockup* me)
{
    if (!igBeginMainMenuBar())
        return;

    if (igBeginMenu("PCMockup", true))
    {
        if (igMenuItemBool("Exit", NULL, false, true))
            me->isRunning = false;
        igEndMenu();
    }
    debugWindowSet_updateMenubar(me->debugWindowSet);

    igEndMainMenuBar();
}

void pcmockup_update(PCMockup *me)
{
    windowContainer_startUpdate(me->windowContainer);
    pcmockup_updateMainMenubar(me);
    windowContainer_endUpdate(me->windowContainer);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            me->isRunning = 0;
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case (SDLK_ESCAPE):
            {
                me->isRunning = 0;
            }break;
            }
        }
        windowContainer_handleEvent(me->windowContainer, &event);
    }
}

void pcmockup_mainLoop(PCMockup *me)
{
    while (me->isRunning)
    {
        const uint32_t frameStart = SDL_GetTicks();

        pcmockup_update(me);

        const uint32_t frameEnd = SDL_GetTicks();
        const int delay = (1000 / MAX_FRAMERATE) - (int)(frameEnd - frameStart);
        if (delay > 0)
            SDL_Delay(delay);
    }
}

#undef main
int main(int argc, char *argv[])
{
    UNUSED(argc, argv);
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    if (SDL_GL_LoadLibrary(NULL) < 0)
    {
        fprintf(stderr, "SDL_GL_LoadLibrary: %s\n", SDL_GetError());
        return -1;
    }
    PCMockup* pcmockup = pcmockup_init();
    if (pcmockup == NULL)
        return -1;

    pcmockup_mainLoop(pcmockup);

    pcmockup_free(pcmockup);
    SDL_Quit();
    return 0;
}

SDL_Rect findBestFit(SDL_Rect src, float dstAspect)
{
    SDL_Rect dst;
    const float srcAspect = (float)src.w / src.h;
    if (srcAspect > dstAspect)
    {
        dst.y = 0;
        dst.h = src.h;
        dst.w = (int)(dstAspect * src.h);
        dst.x = (src.w / 2) - (dst.w / 2);
    }
    else
    {
        dst.x = 0;
        dst.w = src.w;
        dst.h = (int)(src.w / dstAspect);
        dst.y = (src.h / 2) - (dst.h / 2);
    }
    return dst;
}

SDL_Rect padRect(SDL_Rect rect, GSize amount)
{
    return (SDL_Rect) {
        rect.x + amount.w / 2,
        rect.y + amount.h / 2,
        rect.w - amount.w,
        rect.h - amount.h
    };
}

Uint32 getWindowIDByEvent(const SDL_Event* ev)
{
    switch (ev->type)
    {
        case (SDL_KEYDOWN):
        case (SDL_KEYUP):
            return ev->key.windowID;
        case (SDL_MOUSEBUTTONDOWN):
        case (SDL_MOUSEBUTTONUP):
            return ev->button.windowID;
        case (SDL_MOUSEMOTION):
            return ev->motion.windowID;
        case (SDL_MOUSEWHEEL):
            return ev->wheel.windowID;
        case (SDL_WINDOWEVENT):
            return ev->window.windowID;
        case (SDL_TEXTINPUT):
            return ev->text.windowID;
    }
    return UINT32_MAX;
}
