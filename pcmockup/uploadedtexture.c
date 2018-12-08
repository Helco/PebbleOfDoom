#include "pcmockup.h"

struct UploadedTexture {
    GLuint textureId;
    GSize size;
    SDL_PixelFormat* pixelFormat;
};

UploadedTexture* uploadedTexture_init()
{
    UploadedTexture* me = (UploadedTexture*)malloc(sizeof(UploadedTexture));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(UploadedTexture));

    me->pixelFormat = SDL_AllocFormat(imageWindow_SDLPixelFormat);
    if (me->pixelFormat == NULL)
    {
        fprintf(stderr, "SDL_AllocFormat: %s\n", SDL_GetError());
        uploadedTexture_free(me);
        return NULL;
    }

    glGenTextures(1, &me->textureId);
    if (me->textureId == 0)
    {
        fprintf(stderr, "glGenTextures: %d\n", glGetError());
        uploadedTexture_free(me);
        return NULL;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, me->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    const SDL_Color white = { 255, 255, 255, 255 };
    uploadedTexture_setFrom32Bit(me, (GSize) { 1, 1 }, &white);
    return me;
}

void uploadedTexture_free(UploadedTexture* me)
{
    if (me == NULL)
        return;
    if (me->textureId != 0)
        glDeleteTextures(1, &me->textureId);
    if (me->pixelFormat != NULL)
        SDL_FreeFormat(me->pixelFormat);
    free(me);
}

GLuint uploadedTexture_getGLTextureId(UploadedTexture* me)
{
    return me->textureId;
}

GSize uploadedTexture_getSize(UploadedTexture* me)
{
    return me->size;
}

void uploadedTexture_setFrom32Bit(UploadedTexture* me, GSize size, const SDL_Color* pixels)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, me->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    me->size = size;
}

static inline SDL_Color prv_convertGColorTo32Bit(GColor pebbleColor)
{
    SDL_Color color;
    color.r = pebbleColor.r * (255 / 3);
    color.g = pebbleColor.g * (255 / 3);
    color.b = pebbleColor.b * (255 / 3);
    color.a = pebbleColor.a * (255 / 3);
    return color;
}

void uploadedTexture_setFrom8Bit(UploadedTexture* me, GSize size, const GColor* pebblePixels)
{
    SDL_Color* texPixels = (SDL_Color*)malloc(sizeof(SDL_Color) * size.w * size.h);
    assert(texPixels != NULL);

    uint32_t* itTexPixel;
    const GColor* itPebblePixel;
    for (int y = 0; y < size.h; y++)
    {
        itTexPixel = (uint32_t*)(texPixels + y * size.w);
        for (int x = 0; x < size.w; x++)
        {
            itPebblePixel = pebblePixels + x * size.h + y;
            SDL_Color color = prv_convertGColorTo32Bit(*itPebblePixel);
            *(itTexPixel++) = SDL_MapRGBA(me->pixelFormat,
                color.r, color.g, color.b, color.a);
        }
    }

    uploadedTexture_setFrom32Bit(me, size, texPixels);
    free(texPixels);
}

void uploadedTexture_setFromTexture(UploadedTexture* me, const Texture* texture)
{
    uploadedTexture_setFrom8Bit(me, texture->size, texture->pixels);
}
