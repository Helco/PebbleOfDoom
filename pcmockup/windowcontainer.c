#include "window_internal.h"
#include <assert.h>
#include <glad/glad.h>
#include "cimgui.include.h"
#include "sdl.include.h"

#define IMGUI_IMPL_API
#include <imgui_impl_sdl.h>

// glsl_version is default-defined in the header file, so we have to
// declare them ourself without this default value
extern int ImGui_ImplOpenGL3_Init(const char* glsl_version);
extern void ImGui_ImplOpenGL3_Shutdown();
extern void ImGui_ImplOpenGL3_NewFrame();
extern void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);

#define WINDOW_CONTAINER_CHUNK 16

struct WindowContainer
{
    SDL_Window* window;
    SDL_GLContext glContext;
    ImGuiContext* imGuiContext;
    Window** windows;
    Window* focusedWindow;
    int windowCount, windowCapacity;
};

WindowContainer* windowContainer_init(GSize windowSize)
{
    WindowContainer* me = (WindowContainer*)malloc(sizeof(WindowContainer));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(WindowContainer));

    me->window = SDL_CreateWindow("PebbleOfDoom",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowSize.w, windowSize.h,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (me->window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        windowContainer_free(me);
        return NULL;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(0);
    me->glContext = SDL_GL_CreateContext(me->window);
    if (me->glContext == NULL)
    {
        fprintf(stderr, "SDL_GL_CreateContext: %s\n", SDL_GetError());
        windowContainer_free(me);
        return NULL;
    }

    SDL_GL_MakeCurrent(me->window, me->glContext);
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "gladLoadGLLoader: %s\n", SDL_GetError());
        windowContainer_free(me);
        return NULL;
    }

    me->imGuiContext = igCreateContext(NULL);
    if (me->imGuiContext == NULL)
    {
        fprintf(stderr, "igCreateContext: failure\n");
        windowContainer_free(me);
        return NULL;
    }
    ImGuiIO* io = igGetIO();
    io->IniFilename = NULL;

    if (!ImGui_ImplOpenGL3_Init(NULL))
    {
        fprintf(stderr, "ImGui_ImplOpenGL3_Init: failure\n");
        windowContainer_free(me);
        return NULL;
    }

    if (!ImGui_ImplSDL2_InitForOpenGL(me->window, me->glContext))
    {
        fprintf(stderr, "ImGui_ImplSDL2_InitForOpenGL: failure\n");
        windowContainer_free(me);
        return NULL;
    }

    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    return me;
}

void windowContainer_free(WindowContainer* me)
{
    if (me == NULL)
        return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    if (me->imGuiContext != NULL)
        igDestroyContext(me->imGuiContext);
    if (me->glContext)
        SDL_GL_DeleteContext(me->glContext);
    if (me->window)
        SDL_DestroyWindow(me->window);
    if (me->windows != NULL)
    {
        for (int i = 0; i < me->windowCount; i++)
            window_free(me->windows[i]);
        free(me->windows);
    }
    free(me);
}

void windowContainer_startUpdate(WindowContainer* me)
{
    bool open = true;
    SDL_GL_MakeCurrent(me->window, me->glContext);
    igSetCurrentContext(me->imGuiContext);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(me->window);
    igNewFrame();
    igShowDemoWindow(&open);

    me->focusedWindow = NULL;
    for (int i = 0; i < me->windowCount; i++)
    {
        window_update(me->windows[i]);
        if (window_isFocused(me->windows[i]))
            me->focusedWindow = me->windows[i];
    }
}

void windowContainer_endUpdate(WindowContainer* me)
{
    SDL_GL_MakeCurrent(me->window, me->glContext);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    glFlush();
    SDL_GL_SwapWindow(me->window);
}

Window* windowContainer_newWindow(WindowContainer* me, const char* title)
{
    if (me->windowCount == me->windowCapacity)
    {
        me->windowCapacity += WINDOW_CONTAINER_CHUNK;
        me->windows = (Window**)realloc(me->windows, sizeof(Window*) * me->windowCapacity);
        assert(me->windows != NULL);
    }
    Window* newWindow = window_init();
    window_setTitle(newWindow, title);
    me->windows[me->windowCount++] = newWindow;
    return newWindow;
}

Window* windowContainer_getFocusedWindow(WindowContainer* me)
{
    return me->focusedWindow;
}

void windowContainer_handleEvent(WindowContainer* me, const SDL_Event* ev)
{
    // the ImGui SDL2 implementation does not filter the events by window ID
    Uint32 windowID = SDL_GetWindowID(me->window);
    if (getWindowIDByEvent(ev) != windowID)
        return;
    ImGui_ImplSDL2_ProcessEvent((SDL_Event*)ev);
    // TODO: Remove const-removing-cast as soon as (https://github.com/ocornut/imgui/issues/2187) is resolved!

    if (me->focusedWindow == NULL)
        return;
    const ImGuiIO* io = igGetIO();
    if ((ev->type == SDL_KEYDOWN || ev->type == SDL_KEYUP) && !io->WantCaptureKeyboard)
        window_handleKeyEvent(me->focusedWindow, ev->key.keysym, (ev->type == SDL_KEYDOWN));
    window_handleDragEvent(me->focusedWindow);
}
