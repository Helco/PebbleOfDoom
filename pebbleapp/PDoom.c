#include <pebble.h>
#include "textureresources.h"
#include "levelresources.h"
#include "../renderer/renderer.h"
#include "../renderer/segame.h"
#include "../renderer/platform.h"

Window* s_main_window;
Layer* root_layer;
Animation* animation;
Renderer* renderer;
const Level* level;

time_t lastSecond;
uint16_t lastSecondMs;
int curFPS = 0;
static SEGame game;
static GFont font;
static GBitmap* framebuffer_bitmap;
static GContext* ctx;
static void* framebuffer;

int time_difference_ms(time_t a, uint16_t aMs, time_t b, uint16_t bMs)
{
    return
        (a > b ? a - b : -(b - a)) * 1000 +
        (int)(aMs) - bMs;
}

#if PBL_IF_COLOR_ELSE(1, 0)
static uint8_t fauxFramebuffer[20 * 168];
#define W { .r = 3, .g = 3, .b = 3, .a = 3 }
#define B { .r = 0, .g = 0, .b = 0, .a = 3 }

void convertFauxFramebuffer(uint8_t* target)
{
    static union {
        uint32_t pixelQuads[16];
        GColor pixelColors[16 * 4];
    } l = {
        .pixelColors = {
            B, B, B, B,
            W, B, B, B,
            B, W, B, B,
            W, W, B, B,
            B, B, W, B,
            W, B, W, B,
            B, W, W, B,
            W, W, W, B,
            B, B, B, W,
            W, B, B, W,
            B, W, B, W,
            W, W, B, W,
            B, B, W, W,
            W, B, W, W,
            B, W, W, W,
            W, W, W, W,
        }
    };

    uint8_t* source = fauxFramebuffer;
    for (int i = 0; i < 168; i++)
    {
        for (int j = 0; j < 144; j += 8)
        {
            uint8_t s = source[j / 8];
            *((uint32_t*)&target[j + 0]) = l.pixelQuads[s & 0xf];
            *((uint32_t*)&target[j + 4]) = l.pixelQuads[s >> 4];
        }
        source += 20;
        target += 144;
    }
}
#endif

void update_layer(Layer* layer, GContext* gctx)
{
  ctx = gctx;
  framebuffer_bitmap = graphics_capture_frame_buffer(ctx);
  framebuffer = gbitmap_get_data(framebuffer_bitmap);

  segame_update(&game);

  RendererTarget target = {
      .framebuffer = PBL_IF_COLOR_ELSE(fauxFramebuffer, framebuffer),
      .colorFormat = RendererColorFormat_1BitBW
  };
  segame_render(&game, target);
#if PBL_IF_COLOR_ELSE(1, 0)
  convertFauxFramebuffer(framebuffer);
#endif

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

const Sprite* text_sprite_create(TextureManagerHandle _, const char* text)
{
  UNUSED(_);

  if (font == NULL)
    font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_text_color(ctx, GColorWhite);

  GRect box = {
    .origin = { .x = 0, .y = 0 },
    .size = { .w = SCREEN_HEIGHT * 9 / 10, .h = RENDERER_WIDTH * 9 / 10 }
  };
  const GTextOverflowMode overflowMode = GTextOverflowModeWordWrap;
  const GTextAlignment alignment = GTextAlignmentLeft;
  GSize size = graphics_text_layout_get_content_size(text, font, box, overflowMode, alignment);
  size.h += 2; // hack but returned size is apparently wrong

  int bytesPerRow = (size.w + 31) / 32 * 4;
  Sprite* sprite = (Sprite*)malloc(sizeof(Sprite) + bytesPerRow * size.h);
  if (sprite == NULL)
  {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate %d bytes for text sprite %s", sizeof(Sprite) + bytesPerRow * size.h, text);
    return NULL;
  }
  memset(sprite, 0, sizeof(Sprite) + bytesPerRow * size.h);
  sprite->id = -2;
  sprite->size = size;
  sprite->bytesPerRow = bytesPerRow;
  sprite->bw = (uint8_t*)(sprite + 1);
  sprite->alpha = NULL;

//#if PBL_IF_COLOR_ELSE(1, 0)
#if 1
  // game rendering is done to offscreen so we can temporarily abuse the normal framebuffer, given that we clear
  GRect clearBox = {
    .origin = { .x = 0, .y = 0 },
    .size = size
  };
  graphics_release_frame_buffer(ctx, framebuffer_bitmap);
  graphics_fill_rect(ctx, clearBox, 0, 0);
  graphics_draw_text(ctx, text, font, box, overflowMode, alignment, NULL);
  framebuffer_bitmap = graphics_capture_frame_buffer(ctx);

  GColor* source = (GColor*)framebuffer;
  uint8_t* target = sprite->bw;
  for (int y = 0; y < size.h; y++)
  {
    for (int x = 0; x < size.w; x++)
    {
      int byte = x / 8;
      int bit = x % 8;
      target[byte] |= (source[x].r > 0) << bit;
    }
    source += 144;
    target += bytesPerRow;
  }
#else
  // offscreen text rendering based on https://github.com/mhungerford/pebble_offscreen_rendering_text_demo/blob/master/src/main.c
  uint8_t *orig_addr = gbitmap_get_data(framebuffer_bitmap);
  GBitmapFormat orig_format = gbitmap_get_format(framebuffer_bitmap);
  uint16_t orig_stride = gbitmap_get_bytes_per_row(framebuffer_bitmap);
  graphics_release_frame_buffer(ctx, framebuffer_bitmap);
  gbitmap_set_data(framebuffer_bitmap, sprite->bw, GBitmapFormat1Bit, sprite->bytesPerRow, false);
  graphics_draw_text(ctx, text, font, box, overflowMode, alignment, NULL);
  gbitmap_set_data(framebuffer_bitmap, orig_addr, orig_format, orig_stride, false);

  framebuffer_bitmap = graphics_capture_frame_buffer(ctx);
  framebuffer = gbitmap_get_data(framebuffer_bitmap);
#endif

  return sprite;
}

void text_sprite_free(const Sprite* sprite)
{
  if (sprite == NULL)
    APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free NULL text sprite");
  else if (sprite->id != -2)
    APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free fake text sprite");
  else
    free((void*)sprite);
}

void update_animation(Animation *animation, const AnimationProgress progress)
{
  layer_mark_dirty(root_layer);
}

bool loadTextures()
{
  return 
    loadSpriteFromResource(RESOURCE_ID_SPR_BOOK) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_DIGITS) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_BOOTS) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_GOLD) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_HEART) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_KEY) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_BATTERY) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_FIST) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_USE) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_SPEAK) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_ICON_DOOR) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_BATTERY) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_KEY) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PC) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PC_DEAD) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PC_PUSH) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PT2) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PTR) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PTR_PUSH) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_PTR_DEAD) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_SHOPKEEPER) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_TECHPRIEST) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_TECHPRIEST_ANGERY) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_TECHPRIEST_PUSHED) != INVALID_SPRITE_ID &&
    loadSpriteFromResource(RESOURCE_ID_SPR_TECHPRIEST_DEAD) != INVALID_SPRITE_ID;
}

void on_select_click(ClickRecognizerRef rec, void* context) { segame_input_select_click(&game); }
void on_select_long_click(ClickRecognizerRef rec, void* context) { segame_input_select_long_click(&game); }
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

void trigger_haptic(int length)
{

}

void endApplication()
{
  window_stack_pop_all(false);
}

void setBacklight(int enable)
{
  light_enable(enable);
}

int main(void) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "free: %d", heap_bytes_free());
  if (!loadTextures())
    return -1;
    APP_LOG(APP_LOG_LEVEL_ERROR, "free: %d", heap_bytes_free());
  //TexGenerationContext* texgenctx = texgen_init(NULL, TexGenerator_XOR, PBL_IF_COLOR_ELSE(64, 1));
  //texgen_execute(texgenctx);

  /*LevelId levelId = loadLevelFromResource(RESOURCE_ID_LEVEL_TEST);
  if (levelId == INVALID_LEVEL_ID)
    return -1;

  level = level_load(NULL, levelId);
  if (!level)
    return -1;*/

  renderer = renderer_init();
  renderer_setTextureManager(renderer, NULL);
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, (ClickConfigProvider)click_config_provider);

  segame_init(&game, renderer, NULL);

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
