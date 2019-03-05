#include "textureresources.h"
#include "../renderer/platform.h"

typedef struct LoadedTexture
{
    int referenceCount;
    GBitmap* bitmap;
    Texture texture;
} LoadedTexture;

static LoadedTexture textures[MAX_TEXTURES] = { { 0 } };
static TextureId nextTextureId = 0;

static int findNextTextureSlot()
{
    static TextureId nextSlot = 0;
    if (nextSlot < MAX_TEXTURES && textures[nextSlot].bitmap == NULL)
        return nextSlot++;

    for (nextSlot = 0; nextSlot < MAX_TEXTURES; nextSlot++)
    {
        if (textures[nextSlot].bitmap == NULL)
            return nextSlot++;
    }
    return INVALID_TEXTURE_ID;
}

TextureId loadTextureFromResource(uint32_t resourceId)
{
    int slot = findNextTextureSlot();
    if (slot == INVALID_TEXTURE_ID)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Too many textures are being initialized");
        return INVALID_TEXTURE_ID;
    }

    GBitmap* bitmap = gbitmap_create_with_resource(resourceId);
    if (bitmap == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not load texture resource %d", (int)resourceId);
        return INVALID_TEXTURE_ID;
    }

    GBitmapFormat format = gbitmap_get_format(bitmap);
    if (format != GBitmapFormat8Bit)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Texture bitmap has invalid format %d", format);
        return INVALID_TEXTURE_ID;
    }

    GRect bounds = gbitmap_get_bounds(bitmap);
    textures[slot] = (LoadedTexture) {
        .referenceCount = 0,
        .bitmap = bitmap,
        .texture = {
            .id = nextTextureId++,
            .size = bounds.size,
            .pixels = (GColor *)gbitmap_get_data(bitmap)
        }
    };
    return slot;
}

void freeTextures()
{
    LoadedTexture* curTexture = textures;
    for (int i = 0; i < MAX_TEXTURES; i++, curTexture++)
    {
        if (curTexture->referenceCount != 0)
        {
            APP_LOG(APP_LOG_LEVEL_WARNING, "Texture %d in slot %d was not freed!",
                curTexture->texture.id, i);
        }
        if (curTexture->bitmap != NULL)
            gbitmap_destroy(curTexture->bitmap);
    }
    memset(textures, 0, sizeof(LoadedTexture) * MAX_TEXTURES);
}

static LoadedTexture* findLoadedTextureById(TextureId id)
{
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (textures[i].texture.id == id)
            return &textures[i];
    }
    return NULL;
}

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    UNUSED(manager);
    LoadedTexture* texture = findLoadedTextureById(id);
    const Texture* result = NULL;
    if (texture == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to load invalid texture id %d", id);
    else if (texture->bitmap == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to load freed texture with id %d", id);
    else
    {
        texture->referenceCount++;
        result = &texture->texture;
    }
    return result;
}

const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr)
{
    UNUSED(manager);
    int slot = findNextTextureSlot();
    if (slot == INVALID_TEXTURE_ID)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Too many textures are being initialized");
        return NULL;
    }

    GBitmap* bitmap = gbitmap_create_blank((GSize) { 1, 1 }, GBitmapFormat1Bit); // aplite won't allocate 8bit by its own
    void* data = malloc(size.w * size.h);
    if (bitmap == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate empty texture with size: %d, %d", size.w, size.h);
        return NULL;
    }
    gbitmap_set_data(bitmap, data, GBitmapFormat8Bit, size.w, true);
    gbitmap_set_bounds(bitmap, (GRect) { .origin = { 0, 0 }, .size = size });

    textures[slot] = (LoadedTexture) {
        .referenceCount = 1,
        .bitmap = bitmap,
        .texture = {
            .id = nextTextureId++,
            .size = size,
            .pixels = *contentPtr = (GColor*)data
        }
    };
    return &textures[slot].texture;
}

void texture_free(TextureManagerHandle manager, const Texture* textureToBeFreed)
{
    UNUSED(manager);
    const TextureId id = textureToBeFreed->id;
    LoadedTexture* texture = findLoadedTextureById(id);
    if (texture == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free invalid texture id %d", id);
    else if (texture->bitmap == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free already freed texture with id %d", id);
    else if (--texture->referenceCount == 0)
    {
        gbitmap_destroy(texture->bitmap);
        texture->bitmap = NULL;
        texture->texture.pixels = NULL;
    }
}
