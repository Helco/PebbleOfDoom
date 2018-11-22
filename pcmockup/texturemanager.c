#include "pcmockup.h"
#include <stb_image.h>
#include <assert.h>

#define TEXTURE_MANAGER_CHUNK 256
#define TEXTURE_PATH "data/textures/"
#define TEXTURE_PATH_SIZE 512

typedef struct LoadedTexture
{
    int referenceCount;
    Texture texture;
} LoadedTexture;

struct TextureManager
{
    int count, capacity;
    LoadedTexture* textures;
};

TextureManager* textureManager_init()
{
    const int startCapacity = TEXTURE_MANAGER_CHUNK;
    TextureManager* me = (TextureManager*)malloc(sizeof(TextureManager));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(TextureManager));

    int textureMemorySize = sizeof(LoadedTexture) * startCapacity;
    me->textures = (LoadedTexture*)malloc(textureMemorySize);
    if (me->textures == NULL)
    {
        fprintf(stderr, "Could not allocate start capacity of texture entries\n");
        textureManager_free(me);
        return NULL;
    }
    memset(me->textures, 0, textureMemorySize);

    me->capacity = startCapacity;
    return me;
}

void textureManager_free(TextureManager* me)
{
    if (me == NULL)
        return;
    if (me->textures != NULL)
    {
        LoadedTexture* itTexture = me->textures;
        for (int i = 0; i < me->count; i++, itTexture++)
        {
            assert(itTexture->referenceCount == 0);
            if (itTexture->texture.pixels != NULL)
                free(itTexture->texture.pixels);
        }
        free(me->textures);
    }
    free(me);
}

static LoadedTexture* prv_textureManager_nextEntry(TextureManager* me)
{
    if (me->count == me->capacity) {
        int newCount = me->count + TEXTURE_MANAGER_CHUNK;
        LoadedTexture* newTextures = (LoadedTexture*)realloc(me->textures, sizeof(LoadedTexture) * newCount);
        if (newTextures == NULL)
            return NULL;
        me->count = newCount;
        me->textures = newTextures;
    }
    LoadedTexture *texture = &me->textures[me->count];
    texture->referenceCount = 0;
    texture->texture.id = me->count;
    me->count++;
    return texture;
}

void prv_convertRGBAtoPebbleImage(const uint8_t* rgbaPixels, GColor* pebblePixels, int width, int height)
{
    int pixelCount = width * height;
    const uint8_t* itRGBAPixel = rgbaPixels;
    GColor* itPebblePixel = pebblePixels;
    while (pixelCount--)
    {
        itPebblePixel->r = itRGBAPixel[0] / (255 / 3);
        itPebblePixel->g = itRGBAPixel[1] / (255 / 3);
        itPebblePixel->b = itRGBAPixel[2] / (255 / 3);
        itPebblePixel->a = itRGBAPixel[3] / (255 / 3);
        itPebblePixel++;
        itRGBAPixel += 4;
    }
}

TextureId textureManager_registerFile(TextureManager* me, const char* filename)
{
    static char pathBuffer[TEXTURE_PATH_SIZE];
    snprintf(pathBuffer, TEXTURE_PATH_SIZE, TEXTURE_PATH "%s", filename);

    int width, height;
    uint8_t* rgbaPixels = stbi_load(pathBuffer, &width, &height, NULL, 4);
    if (rgbaPixels == NULL)
    {
        fprintf(stderr, "Could not register texture file: %s\n", pathBuffer);
        return INVALID_TEXTURE_ID;
    }

    int textureMemorySize = sizeof(GColor) * width * height;
    GColor* pebblePixels = (GColor*)malloc(textureMemorySize);
    if (pebblePixels == NULL)
    {
        fprintf(stderr, "Could not allocate %d bytes for texture: %s\n", textureMemorySize, pathBuffer);
        stbi_image_free(rgbaPixels);
        return INVALID_TEXTURE_ID;
    }
    prv_convertRGBAtoPebbleImage(rgbaPixels, pebblePixels, width, height);

    LoadedTexture* loadedTex = prv_textureManager_nextEntry(me);
    if (loadedTex == NULL)
    {
        fprintf(stderr, "Could not allocate texture entry\n");
        stbi_image_free(rgbaPixels);
        free(pebblePixels);
        return INVALID_TEXTURE_ID;
    }
    loadedTex->referenceCount = 0;
    loadedTex->texture.size.w = width;
    loadedTex->texture.size.h = height;
    loadedTex->texture.pixels = pebblePixels;
    return loadedTex->texture.id;
}

const Texture* textureManager_loadTexture(TextureManager* me, TextureId id)
{
    assert(id >= 0 && id < me->count && "Tried to load invalid texture id");
    LoadedTexture* loadedTexture = &me->textures[id];
    loadedTexture->referenceCount++;
    return &loadedTexture->texture;
}

void textureManager_freeTexture(TextureManager* me, const Texture* texture)
{
    assert(texture != NULL && texture->id >= 0 && texture->id < me->count &&
        "Tried to free invalid texture");
    LoadedTexture* loadedTexture = &me->textures[texture->id];
    assert(loadedTexture->referenceCount > 0);
    loadedTexture->referenceCount--;
}
