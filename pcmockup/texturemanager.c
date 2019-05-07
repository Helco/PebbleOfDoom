#define _CRT_NONSTDC_NO_DEPRECATE
#include "pcmockup.h"
#include <string.h>
#include <stb_image.h>
#include <assert.h>

#define TEXTURE_MANAGER_CHUNK 256
#define TEXTURE_PATH_SIZE 512

typedef struct LoadedTexture
{
    int referenceCount;
    Texture texture;
    char* source;
    TexGenerationContext* generationContext;
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
            if (itTexture->generationContext != NULL)
                texgen_free(itTexture->generationContext);

            assert(itTexture->referenceCount == 0);
            if (itTexture->texture.pixels != NULL)
                free(itTexture->texture.pixels);
            if (itTexture->source != NULL)
                free(itTexture->source);
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
    texture->generationContext = NULL;
    texture->source = NULL;
    me->count++;
    return texture;
}

void prv_convertRGBAtoPebbleImage(const uint8_t* rgbaPixels, GColor* pebblePixels, int width, int height)
{
    const uint8_t* itRGBAPixel = rgbaPixels;
    GColor* itPebblePixel = pebblePixels;
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            itPebblePixel = pebblePixels + y * width + x;
            itPebblePixel->r = itRGBAPixel[0] / (255 / 3);
            itPebblePixel->g = itRGBAPixel[1] / (255 / 3);
            itPebblePixel->b = itRGBAPixel[2] / (255 / 3);
            itPebblePixel->a = itRGBAPixel[3] / (255 / 3);
            itRGBAPixel += 4;
        }
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
    stbi_image_free(rgbaPixels);

    LoadedTexture* loadedTex = prv_textureManager_nextEntry(me);
    if (loadedTex == NULL)
    {
        fprintf(stderr, "Could not allocate texture entry\n");
        free(pebblePixels);
        return INVALID_TEXTURE_ID;
    }
    loadedTex->referenceCount = 0;
    loadedTex->texture.size.w = width;
    loadedTex->texture.size.h = height;
    loadedTex->texture.pixels = pebblePixels;
    loadedTex->source = strdup(filename);
    return loadedTex->texture.id;
}

const Texture* textureManager_loadTexture(TextureManager* me, TextureId id)
{
    assert(id >= 0 && id < me->count && "Tried to load invalid texture id");
    LoadedTexture* loadedTexture = &me->textures[id];
    loadedTexture->referenceCount++;
    return &loadedTexture->texture;
}

const Texture* textureManager_createEmptyTexture(TextureManager* me, GSize size, GColor** outputPtr)
{
    assert(size.w >= 0 && size.h >= 0);
    LoadedTexture* loadedTexture = prv_textureManager_nextEntry(me);
    if (loadedTexture == NULL)
        return NULL;
    loadedTexture->referenceCount = 1;
    textureManager_resizeEmptyTexture(me, loadedTexture->texture.id, size, outputPtr);
    return &loadedTexture->texture;
}

void textureManager_resizeEmptyTexture(TextureManager* me, TextureId id, GSize newSize, GColor** outputPtr)
{
    assert(id >= 0 && id < me->count);
    assert(newSize.w >= 0 && newSize.h >= 0);

    LoadedTexture* loadedTexture = &me->textures[id];
    if (loadedTexture->texture.pixels != NULL)
        free(loadedTexture->texture.pixels);
    loadedTexture->texture.pixels = (GColor*)malloc(sizeof(GColor) * newSize.w * newSize.h);
    loadedTexture->texture.size = newSize;

    if (outputPtr != NULL)
        *outputPtr = loadedTexture->texture.pixels;
}

void textureManager_freeTexture(TextureManager* me, const Texture* texture)
{
    assert(texture != NULL && texture->id >= 0 && texture->id < me->count &&
        "Tried to free invalid texture");
    LoadedTexture* loadedTexture = &me->textures[texture->id];
    assert(loadedTexture->referenceCount > 0);
    loadedTexture->referenceCount--;
}

TexGenerationContext* textureManager_createGeneratedTexture(TextureManager* me, TexGeneratorID id, int size)
{
    TexGenerationContext* generationContext = texgen_init(me, id, size);
    if (generationContext == NULL)
        return NULL;
    LoadedTexture* loadedTexture = &me->textures[texgen_getTextureId(generationContext)];
    loadedTexture->generationContext = generationContext;
    loadedTexture->source = strdup("<generated>");
    return generationContext;
}

TexGenerationContext* textureManager_getGenerationContext(TextureManager* me, TextureId id)
{
    assert(id >= 0 && id < me->count);
    return me->textures[id].generationContext;
}

int textureManager_getTextureCount(TextureManager* me)
{
    return me->count;
}

const Texture* textureManager_getTextureByIndex(TextureManager* me, int publicIndex)
{
    return publicIndex < 0 || publicIndex >= me->count ? NULL
        : &me->textures[publicIndex].texture;
}

const char* textureManager_getTextureSource(TextureManager* me, const Texture* texture)
{
    static const char* const EMPTY = "";
    LoadedTexture* loadedTexture = &me->textures[texture->id];
    return loadedTexture->source == NULL ? EMPTY : loadedTexture->source;;
}
