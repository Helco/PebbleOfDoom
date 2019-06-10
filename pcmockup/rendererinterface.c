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

const Level* level_load(LevelManagerHandle manager, LevelId id)
{
    LevelManager* levelManager = (LevelManager*)manager;
    return levelManager_getLevelByIndex(levelManager, (int)id);
}

void level_free(LevelManagerHandle manager, const Level* level)
{
    UNUSED(manager, level);
}
