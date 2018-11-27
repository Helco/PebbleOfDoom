#include <pebble.h>
#include "textureresources.h"
#include "../renderer/renderer.h"

Window* s_main_window;
Layer* root_layer;
Animation* animation;
Renderer* renderer;

void update_layer(Layer* layer, GContext* ctx)
{
  GBitmap* framebuffer_bitmap = graphics_capture_frame_buffer(ctx);
  GColor* framebuffer = (GColor*)gbitmap_get_data(framebuffer_bitmap);

  renderer_render(renderer, framebuffer);

  graphics_release_frame_buffer(ctx, framebuffer_bitmap);
}

void update_animation(Animation *animation, const AnimationProgress progress)
{
  layer_mark_dirty(root_layer);
}

bool loadTextures()
{
  static const uint32_t resourceIds[] = {
    RESOURCE_ID_TEXTURE_XOR64
  };
  static const int countIds = sizeof(resourceIds) / sizeof(uint32_t);
  for (int i = 0; i < countIds; i++)
  {
    if (loadTextureFromResource(resourceIds[i]))
      return false;
  }
  return true;
}

int main(void) {
  if (!loadTextures())
    return -1;

  renderer = renderer_init();
  s_main_window = window_create();

  root_layer = window_get_root_layer(s_main_window);
  layer_set_update_proc(root_layer, update_layer);

  animation = animation_create();
  animation_set_play_count(animation, ANIMATION_PLAY_COUNT_INFINITE);
  animation_set_duration(animation, 1000);
  AnimationImplementation impl = {
    .setup = NULL,
    .teardown = NULL,
    .update = update_animation
  };
  animation_set_implementation(animation, &impl);
  animation_schedule(animation);

  window_stack_push(s_main_window, true);
  app_event_loop();

  freeTextures();
}
