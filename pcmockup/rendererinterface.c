#include "pcmockup.h"

// This file should contain simple delegations from
// renderer interface functions to pcmockup ones.

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_loadTexture(textureManager, id);
}

extern const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_createEmptyTexture(textureManager, size, contentPtr);
}

extern void texture_resizeEmpty(TextureManagerHandle manager, TextureId id, GSize newSize, GColor** contentPtr)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_resizeEmptyTexture(textureManager, id, newSize, contentPtr);
}

void texture_free(TextureManagerHandle manager, const Texture* texture)
{
    TextureManager* textureManager = (TextureManager*)manager;
    textureManager_freeTexture(textureManager, texture);
}
