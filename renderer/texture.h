#ifndef TEXTURE_H
#define TEXTURE_H
#include "algebra.h"
#include <pebble.h>

#define INVALID_TEXTURE_ID ((TextureId)-1)
#define INVALID_SPRITE_ID ((SpriteId)-1)

typedef int TextureId;
typedef int SpriteId;
typedef void* TextureManagerHandle;

typedef struct Texture {
    TextureId id;
    GSize size;
    GColor* pixels;
} Texture;

extern const Texture* texture_load(TextureManagerHandle manager, TextureId id);
extern const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr);
extern void texture_resizeEmpty(TextureManagerHandle manager, TextureId id, GSize newSize, GColor** contentPtr);
extern void texture_free(TextureManagerHandle manager, const Texture* texture);

struct SpriteBW {
    SpriteId id;
    GSize size;
    int bytesPerRow;
    uint8_t* bw;
    uint8_t* alpha;
};
typedef struct SpriteBW Sprite;
extern const Sprite* sprite_load(TextureManagerHandle manager, SpriteId id);
extern void sprite_free(TextureManagerHandle manager, const Sprite* sprite);
extern void sprite_clearMemory(TextureManagerHandle manager);

extern const Sprite* text_sprite_create(TextureManagerHandle manager, const char* text);
extern void text_sprite_free(const Sprite* sprite);

extern void trigger_haptic(int length);

extern void endApplication();

#endif
