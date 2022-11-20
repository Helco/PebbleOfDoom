#include <pebble.h>
#include "textureresources.h"
#include "levelresources.h"
#include "../renderer/renderer.h"
#include "../renderer/texgen/texgen.h"
#include "../renderer/segame.h"

Window* s_main_window;
Layer* root_layer;
Animation* animation;
Renderer* renderer;
const Level* level;

time_t lastSecond;
uint16_t lastSecondMs;
int curFPS = 0;
static SEGame game;

int time_difference_ms(time_t a, uint16_t aMs, time_t b, uint16_t bMs)
{
    return
        (a > b ? a - b : -(b - a)) * 1000 +
        (int)(aMs) - bMs;
}

void update_layer(Layer* layer, GContext* ctx)
{
  GBitmap* framebuffer_bitmap = graphics_capture_frame_buffer(ctx);
  void* framebuffer = gbitmap_get_data(framebuffer_bitmap);

  segame_update(&game);

  RendererTarget target = {
      .framebuffer = framebuffer,
      .colorFormat = PBL_IF_COLOR_ELSE(
          RendererColorFormat_8BitColor,
          RendererColorFormat_1BitBW
      )
  };
  renderer_render(renderer, target);
  segame_render(&game, target);

  graphics_release_frame_buffer(ctx, framebuffer_bitmap);

  // Update FPS counter
  time_t curTime;
  uint16_t curTimeMs;
  time_ms(&curTime, &curTimeMs);
  if (time_difference_ms(curTime, curTimeMs, lastSecond, lastSecondMs) >= 1000)
  {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Framerate: %d", curFPS);
      curFPS = 0;
      lastSecond = curTime;
      lastSecondMs = curTimeMs;
  }
  curFPS++;
}

void update_animation(Animation *animation, const AnimationProgress progress)
{
  layer_mark_dirty(root_layer);
}

bool loadTextures()
{
  static const uint32_t resourceIds[] = {
#if PBL_IF_COLOR_ELSE(1, 0)
      RESOURCE_ID_TEXTURE_INFODESK,
      RESOURCE_ID_TEXTURE_CANTINA,
      RESOURCE_ID_TEXTURE_ELEVATORS,
      RESOURCE_ID_TEXTURE_BLACKBOARD,
      RESOURCE_ID_TEXTURE_ENTRY
#endif
  };
  static const int countIds = sizeof(resourceIds) / sizeof(uint32_t);
  for (int i = 0; i < countIds; i++)
  {
    TextureId texId = loadTextureFromResource(resourceIds[i]);
    if (texId == INVALID_TEXTURE_ID)
      return false;
    texture_load(NULL, texId);
  }
  return 
    loadSpriteFromResource(RESOURCE_ID_SPR_BOOK) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_DIGITS) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_BOOTS) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_GOLD) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_HEART) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_KEY) != INVALID_SPRITE_ID;
}

void on_select_click(ClickRecognizerRef rec, void* context) { segame_input_select_click(&game); }
void on_select_long_click(ClickRecognizerRef rec, void* context) { }
void on_back_click(ClickRecognizerRef rec, void* context) { segame_input_back_click(&game); }
void on_right_click(ClickRecognizerRef rec, void* context) { segame_input_direction_click(&game, true); }
void on_left_click(ClickRecognizerRef rec, void* context) { segame_input_direction_click(&game, false); }
void on_right_up(ClickRecognizerRef rec, void* context) { segame_input_direction_raw(&game, true, false); }
void on_right_down(ClickRecognizerRef rec, void* context) { segame_input_direction_raw(&game, true, true); }
void on_left_up(ClickRecognizerRef rec, void* context) { segame_input_direction_raw(&game, false, false); }
void on_left_down(ClickRecognizerRef rec, void* context) { segame_input_direction_raw(&game, false, true); }

void click_config_provider(Window* window)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, on_select_click);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500, on_select_long_click, NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, on_back_click);
  window_single_click_subscribe(BUTTON_ID_DOWN, on_right_click);
  window_single_click_subscribe(BUTTON_ID_UP, on_left_click);
  window_raw_click_subscribe(BUTTON_ID_DOWN, on_right_down, on_right_up, NULL);
  window_raw_click_subscribe(BUTTON_ID_UP, on_left_down, on_left_up, NULL);
}

int main(void) {
  if (!loadTextures())
    return -1;
  //TexGenerationContext* texgenctx = texgen_init(NULL, TexGenerator_XOR, PBL_IF_COLOR_ELSE(64, 1));
  //texgen_execute(texgenctx);

  LevelId levelId = loadLevelFromResource(RESOURCE_ID_LEVEL_TEST);
  if (levelId == INVALID_LEVEL_ID)
    return -1;

  level = level_load(NULL, levelId);
  if (!level)
    return -1;

  renderer = renderer_init();
  renderer_setLevel(renderer, level);
  renderer_setTextureManager(renderer, NULL);
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, (ClickConfigProvider)click_config_provider);

  segame_init(&game, renderer);

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

  time_ms(&lastSecond, &lastSecondMs);

  window_stack_push(s_main_window, true);
  app_event_loop();

  segame_free(&game);
  renderer_free(renderer);
  level_free(NULL, level);
  freeTextures();
}
