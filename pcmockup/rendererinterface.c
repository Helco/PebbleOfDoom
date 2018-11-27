#include "pcmockup.h"

// This file should contain simple delegations from
// renderer interface functions to pcmockup ones.

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    TextureManager* textureManager = (TextureManager*)manager;
    return textureManager_loadTexture(textureManager, id);
}

void texture_free(TextureManagerHandle manager, const Texture* texture)
{
    TextureManager* textureManager = (TextureManager*)manager;
    textureManager_freeTexture(textureManager, texture);
}
