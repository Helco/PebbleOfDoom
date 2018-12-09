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
    WindowContainer* windowContainer;
    bool_t isRunning;
};

bool pcmockup_initDebugWindowSDL(WindowContainer* parent, GRect bounds, const DebugView* view, Renderer* renderer);
bool pcmockup_initDebugWindowImGui(WindowContainer* parent, GRect bounds, const DebugView* view, Renderer* renderer);
static const DebugWindowConstructor debugWindowConstructors[] = {
    [DebugViewType_SDL] = pcmockup_initDebugWindowSDL,
    [DebugViewType_ImGui] = pcmockup_initDebugWindowImGui
};

void pcmockup_updateMainMenubar(void* userdata);

static int countOpenDebugWindows(Renderer* renderer)
{
    int count = 0;
    for (int i = 0; i < renderer_getDebugCount(renderer); i++) {
        if (renderer_getDebugViews(renderer)[i].startsOpened)
            count++;
    }
    return count;
}

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
    windowContainer_addMenubarHandler(me->windowContainer,
        pcmockup_updateMainMenubar, "PCMockup", me);

    WindowGrid windowGrid;
    windowGrid.windowCount = 1 + countOpenDebugWindows(me->renderer);
    windowGrid.totalSize = WINDOW_START_SIZE;

    PebbleWindow* pebbleWindow = pebbleWindow_init(
        me->windowContainer,
        windowGrid_getSingleBounds(&windowGrid, 0),
        GSize(RENDERER_WIDTH, RENDERER_HEIGHT),
        RendererColorFormat_1BitBW,
        me->renderer
    );
    if (pebbleWindow == NULL)
    {
        pcmockup_free(me);
        return NULL;
    }

    int gridPlace = -1;
    for (int i = 0; i < renderer_getDebugCount(me->renderer); i++)
    {
        const DebugView* debugView = &renderer_getDebugViews(me->renderer)[i];
        const int curGridPlace = debugView->startsOpened ? gridPlace-- : -1 - i;
        if (!debugWindowConstructors[debugView->type](
            me->windowContainer,
            windowGrid_getSingleBounds(&windowGrid, curGridPlace),
            debugView, me->renderer)) {
            pcmockup_free(me);
            return NULL;
        }
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
    if (me->level != NULL)
        level_free(me->level);
    if (me->textureManager != NULL)
        textureManager_free(me->textureManager);
    if (me->renderer != NULL)
        renderer_free(me->renderer);
    free(me);
}

void pcmockup_updateMainMenubar(void* userdata)
{
    PCMockup* me = (PCMockup*)userdata;
    if (igMenuItemBool("Exit", NULL, false, true))
        me->isRunning = false;
}

void pcmockup_update(PCMockup *me)
{
    windowContainer_update(me->windowContainer);

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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
void pcmockup_emscripten_main_loop(void* userdata) {
    PCMockup* me = (PCMockup*)userdata;
    pcmockup_update(me);
}
#endif

void pcmockup_mainLoop(PCMockup *me)
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(pcmockup_emscripten_main_loop, me, MAX_FRAMERATE, 1);
#else
    while (me->isRunning)
    {
        const uint32_t frameStart = SDL_GetTicks();

        pcmockup_update(me);

        const uint32_t frameEnd = SDL_GetTicks();
        const int delay = (1000 / MAX_FRAMERATE) - (int)(frameEnd - frameStart);
        if (delay > 0)
            SDL_Delay(delay);
    }
#endif
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

GRect findBestFit(GRect src, float dstAspect)
{
    GRect dst;
    const float srcAspect = (float)src.size.w / src.size.h;
    if (srcAspect > dstAspect)
    {
        dst.origin.y = 0;
        dst.size.h = src.size.h;
        dst.size.w = (int)(dstAspect * src.size.h);
        dst.origin.x = (src.size.w / 2) - (dst.size.w / 2);
    }
    else
    {
        dst.origin.x = 0;
        dst.size.w = src.size.w;
        dst.size.h = (int)(src.size.w / dstAspect);
        dst.origin.y = (src.size.h / 2) - (dst.size.h / 2);
    }
    return dst;
}

GRect padRect(GRect rect, GSize amount)
{
    return (GRect) {
        .origin = {
            rect.origin.x + amount.w / 2,
            rect.origin.y + amount.h / 2
        },
        .size = {
            rect.size.w - amount.w,
            rect.size.h - amount.h
        }
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

bool pcmockup_initDebugWindowSDL(WindowContainer* parent, GRect bounds, const DebugView* view, Renderer* renderer)
{
    return debugWindowSDL_init(parent, bounds, view, renderer) != NULL;
}

bool pcmockup_initDebugWindowImGui(WindowContainer* parent, GRect bounds, const DebugView* view, Renderer* renderer)
{
    return debugWindowImGui_init(parent, bounds, view, renderer) != NULL;
}
