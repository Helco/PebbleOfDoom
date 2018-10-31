#include "renderer.h"

struct Renderer
{
    int dummy;
};

Renderer* renderer_init()
{
    Renderer* this = (Renderer*)malloc(sizeof(Renderer));
    if (this == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate renderer");
        return NULL;
    }

    return this;
}

void renderer_free(Renderer* this)
{
    if (this == NULL)
        return;
    free(this);
}

int curvy(int val, int freq, int ampl)
{
    return sin_lookup(val * (1 << 16) / freq) * ampl / (1 << 16);
}

int bouncy(int x, int y, int xFreq, int xAmpl, int ampl)
{
    return curvy(curvy(x, xFreq, xAmpl) * y, xFreq * xAmpl, ampl);
}

/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer)
{
    static int time = 0;
    time++;
    for (int i = 0; i < RENDERER_WIDTH * RENDERER_HEIGHT; i++)
    {
        int x = (i / RENDERER_WIDTH);
        int y = (i % RENDERER_WIDTH) + bouncy(time, x, 80, 50, 64 + curvy(time, 140, 32));
        GColor color;
        color.argb = ((x^y)/ 3 + time) % 256;
        framebuffer[i] = color;
    }
}
