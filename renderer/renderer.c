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

/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer)
{
    
}
