#include "segame.h"
#include "platform.h"

#define DIGIT_WIDTH (HUD_HEIGHT / 2)

void prv_renderIconSrc(RendererTarget target, void* rawSrc, int x, int srcY, int srcH)
{
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* fb = (uint8_t*)target.framebuffer;
    uint32_t* src = (uint32_t*)rawSrc;
    src += srcY;

    fb += x * stride;
    for (int i = 0; i < srcH; i++)
    {
        *((uint32_t*)fb) = *(src++);
        fb += stride;
    }
}

void prv_renderIconSrcInv(RendererTarget target, void* rawSrc, int x, int srcY, int srcH)
{
    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* fb = (uint8_t*)target.framebuffer;
    uint32_t* src = (uint32_t*)rawSrc;
    src += srcY;

    fb += x * stride;
    for (int i = 0; i < srcH; i++)
    {
        *((uint32_t*)fb) = ~*(src++);
        fb += stride;
    }
}

void prv_renderIconPart(RendererTarget target, const Sprite* sprite, int x, int srcY, int srcH)
{
    assert(sprite->size.w == 32);
    assert(sprite->bytesPerRow == 4);
    prv_renderIconSrc(target, sprite->bw, x, srcY, srcH);
}

void prv_renderIcon(RendererTarget target, const Sprite* sprite, int x)
{
    prv_renderIconPart(target, sprite, x, 0, sprite->size.h);
}

void segame_render(SEGame* me, RendererTarget target)
{
    if (!me->hadRenderedBefore)
    {
        const int stride = rendererColorFormat_getStride(target.colorFormat);
        uint8_t* fb = (uint8_t*)target.framebuffer;
        for (int i = 0; i < RENDERER_WIDTH; i++)
            memset(fb + i * stride, 0xff, HUD_HEIGHT / 8);
        me->hadRenderedBefore = true;
    }

    prv_renderIcon(target, me->iconGold, 0);

#define maxGoldText 3
    static char goldBuffer[maxGoldText];
    snprintf(goldBuffer, maxGoldText, "%d", me->player.gold);
    for (int i = 0; i < 3 && goldBuffer[i]; i++)
    {
        int srcY = goldBuffer[i] - '0';
        prv_renderIconPart(target, me->iconDigits, HUD_HEIGHT + i * DIGIT_WIDTH, srcY * DIGIT_WIDTH, DIGIT_WIDTH);
    }

    prv_renderIcon(target, me->iconPlayerActions[me->player.activeAction], RENDERER_WIDTH / 2 - HUD_HEIGHT / 2);

    for (int i = 0; i < me->player.maxHealth; i++)
    {
        int x = RENDERER_WIDTH + DIGIT_WIDTH * (i - me->player.maxHealth);
        bool hasHeart = me->player.health >= me->player.maxHealth - i;
        void* spriteSrc = hasHeart ? me->iconHeart->bw : me->iconHeart->alpha;
        if (hasHeart)
            prv_renderIconSrc(target, spriteSrc, x, (i % 2) * DIGIT_WIDTH, DIGIT_WIDTH);
        else
            prv_renderIconSrcInv(target, spriteSrc, x, (i % 2) * DIGIT_WIDTH, DIGIT_WIDTH);
    }
}
