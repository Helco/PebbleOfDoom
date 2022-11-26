#include "textureresources.h"
#include "../renderer/platform.h"
// silent fail aplite/diorite version

TextureId loadTextureFromResource(uint32_t resourceId)
{
    return 0;
}

void freeTextures()
{
}

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    static GColor pixel = { .argb = 0 };
    static const Texture texture = {
        .id = 0,
        .size = { 1, 1 },
        .pixels = &pixel
    };
    return &texture;
}

const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr)
{
    return NULL;
}

void texture_resizeEmpty(TextureManagerHandle manager, TextureId id, GSize newSize, GColor** contentPtr)
{
}

void texture_free(TextureManagerHandle manager, const Texture* texture)
{
}

typedef struct LoadedSprite
{
    int referenceCount;
    Sprite sprite;
} LoadedSprite;

static LoadedSprite sprites[MAX_TEXTURES] = { { 0 } };

static int findNextSpriteSlot()
{
    static SpriteId nextSlot = 0;
    if (nextSlot < MAX_TEXTURES && sprites[nextSlot].sprite.bw == NULL)
        return nextSlot++;

    for (nextSlot = 0; nextSlot < MAX_TEXTURES; nextSlot++)
    {
        if (sprites[nextSlot].sprite.bw == NULL)
            return nextSlot++;
    }
    return INVALID_SPRITE_ID;
}

SpriteId loadSpriteFromResource(uint32_t resourceId)
{
    int slot = findNextSpriteSlot();
    if (slot == INVALID_SPRITE_ID)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Too many sprites");
        return INVALID_SPRITE_ID;
    }

    GBitmap* bitmap = gbitmap_create_with_resource(resourceId);
    if (bitmap == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not load sprite resource %d", (int)resourceId);
        return INVALID_SPRITE_ID;
    }

    GBitmapFormat format = gbitmap_get_format(bitmap);
    if (format != GBitmapFormat2BitPalette)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Sprite bitmap has invalid format %d", format);
        return INVALID_SPRITE_ID;
    }

    GRect bounds = gbitmap_get_bounds(bitmap);
    if (bounds.origin.x != 0 || bounds.origin.y != 0)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Sprite bitmap has invalid origin");
        return INVALID_SPRITE_ID;
    }

    uint8_t* data = (uint8_t*)gbitmap_get_data(bitmap);
    GColor* palette = (GColor*)gbitmap_get_palette(bitmap);
    int bytesPerRow = gbitmap_get_bytes_per_row(bitmap);
    int myBytesPerRow = (bounds.size.w + 31) / 32 * 4;

    uint8_t* bw = (uint8_t*)malloc(myBytesPerRow * bounds.size.h * 2);
    if (bw == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate %d bytes for sprite", myBytesPerRow * bounds.size.h * 2);
        return INVALID_SPRITE_ID;
    }
    uint8_t* alpha = bw + myBytesPerRow * bounds.size.h;
    memset(bw, 0, myBytesPerRow * bounds.size.h * 2);

    for (int y = 0; y < bounds.size.h; y++)
    {
        for (int x = 0; x < bounds.size.w; x++)
        {
            int bmByte = y * bytesPerRow + x / 4;
            int bmBit = (3 - (x % 4)) * 2;
            GColor bmColor = palette[(data[bmByte] >> bmBit) & 3];

            int sprByte = y * myBytesPerRow + x / 8;
            int sprBit = x % 8;
#if PBL_IF_COLOR_ELSE(1, 0)
            bw[sprByte] |= (bmColor.r > 0 || bmColor.a == 0) << sprBit;
#else
            bw[sprByte] |= (bmColor.r > 0) << sprBit;
#endif
            alpha[sprByte] |= (bmColor.a > 0) << sprBit;
        }
    }

    gbitmap_destroy(bitmap);

    sprites[slot] = (LoadedSprite) {
        .referenceCount = 1,
        .sprite = {
            .id = resourceId,
            .size = bounds.size,
            .bytesPerRow = myBytesPerRow,
            .bw = bw,
            .alpha = alpha
        }
    };
    return slot;
}

void freeSprites()
{
    LoadedSprite* curSprite = sprites;
    for (int i = 0; i < MAX_TEXTURES; i++, curSprite++)
    {
        if (curSprite->referenceCount != 0)
        {
            APP_LOG(APP_LOG_LEVEL_WARNING, "Sprite %d in slot %d was not freed!", curSprite->sprite.id, i);
        }
        if (curSprite->sprite.bw != NULL)
        {
            free(curSprite->sprite.bw);
        }
    }
    memset(sprites, 0, sizeof(LoadedSprite) * MAX_TEXTURES);
}

static LoadedSprite* findLoadedSpriteById(SpriteId id)
{
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (sprites[i].sprite.id == id)
            return &sprites[i];
    }
    return NULL;
}

const Sprite* sprite_load(TextureManagerHandle _, SpriteId id)
{
    UNUSED(_);
    LoadedSprite* sprite = findLoadedSpriteById(id);
    const Sprite* result = NULL;
    if (sprite == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to load invalid sprite id %d", id);
    else if (sprite->sprite.bw == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to load freed sprite with id %d", id);
    else
    {
        sprite->referenceCount++;
        result = &sprite->sprite;
    }
    return result;
}

void sprite_free(TextureManagerHandle _, const Sprite* spriteToBeFreed)
{
    UNUSED(_);
    const SpriteId id = spriteToBeFreed->id;
    LoadedSprite* sprite = findLoadedSpriteById(id);
    if (sprite == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free invalid sprite id %d", id);
    else if (sprite->sprite.bw == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to free freed sprite with id %d", id);
    else if (--sprite->referenceCount == 0)
    {
        free(sprite->sprite.bw);
        sprite->sprite.bw = NULL;
        sprite->sprite.alpha = NULL;
    }
}
