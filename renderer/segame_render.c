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

void prv_renderIconInv(RendererTarget target, const Sprite* sprite, int x)
{
    assert(sprite->size.w == 32);
    assert(sprite->bytesPerRow == 4);
    prv_renderIconSrcInv(target, sprite->bw, x, 0, sprite->size.h);
}

void segame_render(SEGame* me, RendererTarget target)
{
    if (!me->isPaused || !me->hadRenderedBefore)
        renderer_render(me->renderer, target);

    if (!me->hadRenderedBefore)
    {
        const int stride = rendererColorFormat_getStride(target.colorFormat);
        uint8_t* fb = (uint8_t*)target.framebuffer;
        for (int i = 0; i < RENDERER_WIDTH; i++)
            memset(fb + i * stride, 0xff, HUD_HEIGHT / 8);
        me->hadRenderedBefore = true;
    }

    if (me->isPaused)
    {
        menu_render(&me->menu, target);
        return;
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

    if (me->player.activeAction == PLAYERACT_WALK && me->player.isWalking)
        prv_renderIconInv(target, me->iconPlayerActions[me->player.activeAction], RENDERER_WIDTH / 2 - HUD_HEIGHT / 2);
    else
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

void prv_render_textSprite(RendererTarget target, const Sprite* sprite, int x, int y)
{
    assert(x >= 0 && y >= 0 && x + sprite->size.w <= RENDERER_WIDTH && y + sprite->size.h <= SCREEN_HEIGHT);
    assert(target.colorFormat == RendererColorFormat_1BitBW);

    const int stride = rendererColorFormat_getStride(target.colorFormat);
    uint8_t* fb = (uint8_t*)target.framebuffer;
    const uint8_t* bw = sprite->bw;
    int xOff = x, yOff = y;

    for (x = 0; x < sprite->size.w; x++)
    {
        for (y = 0; y < sprite->size.h; y++)
        {
            int xByte = x / 8;
            int xBit = x % 8;
            int yByte = (yOff + y) / 8;
            int yBit = (yOff + y) % 8;

            int spriteBit = (bw[(sprite->size.h - 1 - y) * sprite->bytesPerRow + xByte] >> xBit) & 1;
            fb[stride * (xOff + x) + yByte] &= ~(spriteBit << yBit);
        }
    }
}

void prv_render_menuBackground(RendererTarget target, int x, int y, int w, int h)
{
    assert(target.colorFormat == RendererColorFormat_1BitBW);
    assert(x >= 0 && y >= 0 && w > 0 && h > 0);
    assert(x + w <= RENDERER_WIDTH && y + h <= SCREEN_HEIGHT);
    for (int i = 0; i < w; i++)
        render_setBitColumn(target, x + i, y, y + h - 1);
    for (int i = 1; i < w - 1; i++)
    {
        render_clearBitColumn(target, x + i, y + 1, y + 1);
        render_clearBitColumn(target, x + i, y + h - 2, y + h - 2);
    }
    render_clearBitColumn(target, x + 1, y + 1, y + h - 2);
    render_clearBitColumn(target, x + w - 2, y + 1, y + h - 2);
}

void render_flipBitRect(RendererTarget target, GRect rect)
{
    int yStart = rect.origin.y;
    int yEnd = rect.origin.y + rect.size.h - 1;
    for (int x = 0; x < rect.size.w; x++)
        render_flipBitColumn(target, rect.origin.x + x, yStart, yEnd);
}

void menu_render(Menu* me, RendererTarget target)
{
    if (!me->hadBeenRendered)
    {
        TextureManagerHandle textureManager = renderer_getTextureManager(me->game->renderer);
        const Sprite* mainText = text_sprite_create(textureManager, me->text);
        const Sprite* buttonTexts[MAX_BUTTONS] = { 0 };

        me->size = mainText->size;
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            if (me->buttons[i] == NULL)
                break;
            buttonTexts[i] = text_sprite_create(textureManager, me->buttons[i]);
            me->size.w = max(me->size.w, buttonTexts[i]->size.w);
            me->size.h += MENU_BUTTON_SPACE;
        }
        me->size.w += MENU_BORDER * 2;
        me->size.h += MENU_BORDER * 2;

        int x = RENDERER_WIDTH / 2 - me->size.w / 2;
        int y = SCREEN_HEIGHT / 2 - me->size.h / 2;
        prv_render_menuBackground(target, x, y, me->size.w, me->size.h);

        x += MENU_BORDER;
        y += me->size.h - MENU_BORDER - mainText->size.h;
        prv_render_textSprite(target, mainText, x, y + 2);

        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            if (me->buttons[i] == NULL)
                break;
            me->buttonRects[i] = (GRect){
                .origin = {
                    .x = x = RENDERER_WIDTH / 2 - buttonTexts[i]->size.w / 2,
                    .y = y -= MENU_BUTTON_SPACE
                },
                .size = buttonTexts[i]->size
            };
            prv_render_textSprite(target, buttonTexts[i], x, y + 2);
        }

        text_sprite_free(mainText);
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            if (buttonTexts[i] != NULL)
                text_sprite_free(buttonTexts[i]);
        }
        me->hadBeenRendered = true;
    }

    if (me->curButton == me->flippedButton)
        return;

    if (me->flippedButton >= 0)
        render_flipBitRect(target, me->buttonRects[me->flippedButton]);
    render_flipBitRect(target, me->buttonRects[me->curButton]);
    me->flippedButton = me->curButton;
}
