#include <pebble.h>
#include "textureresources.h"
#include "../renderer/renderer.h"
#include "../renderer/texgen/texgen.h"

Window* s_main_window;
Layer* root_layer;
Animation* animation;
Renderer* renderer;
Level* level;

void update_layer(Layer* layer, GContext* ctx)
{
  GBitmap* framebuffer_bitmap = graphics_capture_frame_buffer(ctx);
  void* framebuffer = gbitmap_get_data(framebuffer_bitmap);

  renderer_render(renderer, (RendererTarget) {
      .framebuffer = framebuffer,
      .colorFormat = PBL_IF_COLOR_ELSE(
          RendererColorFormat_8BitColor,
          RendererColorFormat_1BitBW
      )
  });
  renderer_rotate(renderer, rotationRight);

  graphics_release_frame_buffer(ctx, framebuffer_bitmap);
}

void update_animation(Animation *animation, const AnimationProgress progress)
{
  layer_mark_dirty(root_layer);
}

bool loadTextures()
{
  static const uint32_t resourceIds[] = {

  };
  static const int countIds = sizeof(resourceIds) / sizeof(uint32_t);
  for (int i = 0; i < countIds; i++)
  {
    TextureId texId = loadTextureFromResource(resourceIds[i]);
    if (texId == INVALID_TEXTURE_ID)
      return false;
    texture_load(NULL, texId);
  }
  return true;
}

int main(void) {
  if (!loadTextures())
    return -1;
  TexGenerationContext* texgenctx = texgen_init(NULL, TexGenerator_XOR, 64);
  texgen_execute(texgenctx);

  level = level_load(0);
  if (!level)
    return -1;

  renderer = renderer_init();
  renderer_setLevel(renderer, level);
  renderer_setTextureManager(renderer, NULL);
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

  renderer_free(renderer);
  level_free(level);
  freeTextures();
}
