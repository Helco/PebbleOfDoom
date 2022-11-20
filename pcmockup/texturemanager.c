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

typedef struct LoadedSprite
{
    int referenceCount;
    Sprite sprite;
    char* source;
} LoadedSprite;

struct TextureManager
{
    int texCount, texCapacity;
    int spriteCount, spriteCapacity;
    LoadedTexture* textures;
    LoadedSprite* sprites;
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

    int spriteMemorySize = sizeof(LoadedSprite) * startCapacity;
    me->sprites = (LoadedSprite*)malloc(spriteMemorySize);
    if (me->sprites == NULL)
    {
        fprintf(stderr, "Could not allocate start capacity of sprite entries\n");
        textureManager_free(me);
        return NULL;
    }
    memset(me->sprites, 0, spriteMemorySize);

    me->texCapacity = startCapacity;
    me->spriteCapacity = startCapacity;
    return me;
}

void textureManager_free(TextureManager* me)
{
    if (me == NULL)
        return;
    if (me->textures != NULL)
    {
        LoadedTexture* itTexture = me->textures;
        for (int i = 0; i < me->texCount; i++, itTexture++)
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
    if (me->sprites != NULL)
    {
        LoadedSprite* itSprite = me->sprites;
        for (int i = 0; i < me->spriteCount; i++, itSprite++)
        {
            assert(itSprite->referenceCount == 0);
            if (itSprite->sprite.bw != NULL)
                free(itSprite->sprite.bw);
            // alpha is part of the bw block
            if (itSprite->source != NULL)
                free(itSprite->source);
        }
        free(me->sprites);
    }
    free(me);
}

static LoadedTexture* prv_textureManager_nextEntry(TextureManager* me)
{
    if (me->texCount == me->texCapacity) {
        int newCount = me->texCount + TEXTURE_MANAGER_CHUNK;
        LoadedTexture* newTextures = (LoadedTexture*)realloc(me->textures, sizeof(LoadedTexture) * newCount);
        if (newTextures == NULL)
            return NULL;
        me->texCapacity = newCount;
        me->textures = newTextures;
    }
    LoadedTexture *texture = &me->textures[me->texCount];
    texture->referenceCount = 0;
    texture->texture.id = me->texCount;
    texture->generationContext = NULL;
    texture->source = NULL;
    me->texCount++;
    return texture;
}

static LoadedSprite* prv_textureManager_nextSpriteEntry(TextureManager* me)
{
    if (me->spriteCount == me->spriteCapacity) {
        int newCount = me->spriteCount + TEXTURE_MANAGER_CHUNK;
        LoadedSprite* newSprites = (LoadedSprite*)realloc(me->sprites, sizeof(LoadedSprite) * newCount);
        if (newSprites == NULL)
            return NULL;
        me->spriteCapacity = newCount;
        me->sprites = newSprites;
    }
    LoadedSprite* sprite = &me->sprites[me->spriteCount];
    sprite->referenceCount = 0;
    sprite->sprite.id = me->spriteCount;
    sprite->source = NULL;
    me->spriteCount++;
    return sprite;
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

void prv_convertRGBAtoPebbleSprite(const uint8_t* rgbaPixels, uint8_t* bw, uint8_t* alpha, int width, int height, int bytesPerRow)
{
    uint8_t* bwRow = bw;
    uint8_t* alphaRow = alpha;
    const uint8_t* itRGBAPixel = rgbaPixels;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int byte = x / 8;
            int bit = x % 8;
            int mask = (1 << bit);

            bwRow[byte] = (bwRow[byte] & ~mask) | ((itRGBAPixel[0] > 127) << bit);
            alphaRow[byte] = (alphaRow[byte] & ~mask) | ((itRGBAPixel[3] > 127) << bit);
            itRGBAPixel += 4;
        }
        bwRow += bytesPerRow;
        alphaRow += bytesPerRow;
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

SpriteId textureManager_registerSpriteFile(TextureManager* me, const char* filename, SpriteId id)
{
    static char pathBuffer[TEXTURE_PATH_SIZE];
    snprintf(pathBuffer, TEXTURE_PATH_SIZE, TEXTURE_PATH "%s", filename);

    int width, height;
    uint8_t* rgbaPixels = stbi_load(pathBuffer, &width, &height, NULL, 4);
    if (rgbaPixels == NULL)
    {
        fprintf(stderr, "Could not register texture file: %s\n", pathBuffer);
        return INVALID_SPRITE_ID;
    }

    int bytesPerRow = (width + 31) / 32 * 4;
    int spriteMemorySize = bytesPerRow * height;
    uint8_t* bw = (uint8_t*)malloc(spriteMemorySize * 2);
    uint8_t* alpha = bw + spriteMemorySize;
    if (bw == NULL)
    {
        fprintf(stderr, "Could not allocate %d bytes for sprite: %s\n", spriteMemorySize * 2, pathBuffer);
        stbi_image_free(rgbaPixels);
        return INVALID_SPRITE_ID;
    }
    memset(bw, 0, spriteMemorySize * 2);
    prv_convertRGBAtoPebbleSprite(rgbaPixels, bw, alpha, width, height, bytesPerRow);
    stbi_image_free(rgbaPixels);

    LoadedSprite* loadedSprite = prv_textureManager_nextSpriteEntry(me);
    if (loadedSprite == NULL)
    {
        fprintf(stderr, "Could not allocate texture entry\n");
        free(bw);
        return INVALID_SPRITE_ID;
    }
    loadedSprite->referenceCount = 0;
    loadedSprite->sprite.size.w = width;
    loadedSprite->sprite.size.h = height;
    loadedSprite->sprite.bytesPerRow = bytesPerRow;
    loadedSprite->sprite.bw = bw;
    loadedSprite->sprite.alpha = alpha;
    loadedSprite->sprite.id = id;
    loadedSprite->source = strdup(filename);
    return loadedSprite->sprite.id;
}

const Texture* textureManager_loadTexture(TextureManager* me, TextureId id)
{
    assert(id >= 0 && id < me->texCount && "Tried to load invalid texture id");
    LoadedTexture* loadedTexture = &me->textures[id];
    loadedTexture->referenceCount++;
    return &loadedTexture->texture;
}

const Sprite* textureManager_loadSprite(TextureManager* me, SpriteId id)
{
    LoadedSprite* loadedSprite = NULL;
    for (int i = 0; i < me->spriteCount; i++)
    {
        if (me->sprites[i].sprite.id == id)
        {
            loadedSprite = me->sprites + i;
            break;
        }
    }

    assert(loadedSprite != NULL && "Tried to load invalid sprite id");
    loadedSprite->referenceCount++;
    return &loadedSprite->sprite;
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
    assert(id >= 0 && id < me->texCount);
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
    assert(texture != NULL && texture->id >= 0 && texture->id < me->texCount &&
        "Tried to free invalid texture");
    LoadedTexture* loadedTexture = &me->textures[texture->id];
    assert(loadedTexture->referenceCount > 0);
    loadedTexture->referenceCount--;
}

void textureManager_freeSprite(TextureManager* me, const Sprite* sprite)
{
    assert(sprite != NULL && "Tried to free invalid sprite");
    LoadedSprite* loadedSprite = NULL;
    for (int i = 0; i < me->spriteCount; i++)
    {
        if (me->sprites[i].sprite.id == sprite->id)
        {
            loadedSprite = me->sprites + i;
            break;
        }
    }
    assert(loadedSprite != NULL && "Tried to load invalid sprite id");
    assert(loadedSprite->referenceCount > 0);
    loadedSprite->referenceCount--;
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
    assert(id >= 0 && id < me->texCount);
    return me->textures[id].generationContext;
}

int textureManager_getTextureCount(TextureManager* me)
{
    return me->texCount;
}

const Texture* textureManager_getTextureByIndex(TextureManager* me, int publicIndex)
{
    return publicIndex < 0 || publicIndex >= me->texCount ? NULL
        : &me->textures[publicIndex].texture;
}

const char* textureManager_getTextureSource(TextureManager* me, const Texture* texture)
{
    static const char* const EMPTY = "";
    LoadedTexture* loadedTexture = &me->textures[texture->id];
    return loadedTexture->source == NULL ? EMPTY : loadedTexture->source;;
}
