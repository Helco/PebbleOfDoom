#ifndef RENDERER_H
#define RENDERER_H
#include <pebble.h>

#define RENDERER_WIDTH 168
#define RENDERER_HEIGHT 144

typedef struct Renderer Renderer;
Renderer* renderer_init();
void renderer_free(Renderer* renderer);
/*
 * framebuffer - *column-stored* framebuffer pointer
 */
void renderer_render(Renderer* renderer, GColor* framebuffer);

#endif
