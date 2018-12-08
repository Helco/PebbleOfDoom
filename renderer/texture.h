#ifndef TEXTURE_H
#define TEXTURE_H
#include "algebra.h"
#include <pebble.h>

#define INVALID_TEXTURE_ID ((TextureId)-1)

typedef int TextureId;
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

#endif
