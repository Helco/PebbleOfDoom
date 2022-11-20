#include "pcmockup.h"

// This file should contain simple delegations from
// renderer interface functions to pcmockup ones.

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_loadTexture(textureManager, id);
}

const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_createEmptyTexture(textureManager, size, contentPtr);
}

void texture_resizeEmpty(TextureManagerHandle manager, TextureId id, GSize newSize, GColor** contentPtr)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_resizeEmptyTexture(textureManager, id, newSize, contentPtr);
}

void texture_free(TextureManagerHandle manager, const Texture* texture)
{
    TextureManager* textureManager = (TextureManager*)manager;
    textureManager_freeTexture(textureManager, texture);
}

const Sprite* sprite_load(TextureManagerHandle manager, SpriteId id)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_loadSprite(textureManager, id);
}

void sprite_free(TextureManagerHandle manager, const Sprite* sprite)
{
    TextureManager* textureManager = (TextureManager*)manager;
    textureManager_freeSprite(textureManager, sprite);
}

const Level* level_load(LevelManagerHandle manager, LevelId id)
{
    LevelManager* levelManager = (LevelManager*)manager;
    return levelManager_getLevelById(levelManager, id);
}

void level_free(LevelManagerHandle manager, const Level* level)
{
    UNUSED(manager, level);
}


const Sprite* text_sprite_create(TextureManagerHandle manager, const char* text)
{
    UNUSED(manager, text);
    const int w = text[0] != '>' ? 120 : 80;
    const int h = text[0] != '>' ? 50 : 14;
    const int bytesPerRow = (w + 31) / 32 * 4;
    Sprite* sprite = malloc(sizeof(Sprite) + bytesPerRow * h * 2);
    sprite->bw = (uint8_t*)(sprite + 1);
    sprite->alpha = sprite->bw + bytesPerRow * h;
    memset(sprite->alpha, 0xff, bytesPerRow * h);
    memset(sprite->bw, 0xAA, bytesPerRow * h);
    sprite->bytesPerRow = bytesPerRow;
    sprite->size.w = w;
    sprite->size.h = h;
    sprite->id = -2;
    return sprite;
}

void text_sprite_free(const Sprite* sprite)
{
    assert(sprite->id == -2);
    free((void*)sprite);
}
