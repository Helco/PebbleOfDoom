#ifndef TEXTURERESOURCES_H
#define TEXTURERESOURCES_H
#include <pebble.h>
#include "../renderer/texture.h"

#define MAX_TEXTURES 32

TextureId loadTextureFromResource(uint32_t resourceId);
SpriteId loadSpriteFromResource(uint32_t resourceId);
void freeTextures();
void freeSprites();

#endif
