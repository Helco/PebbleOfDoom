#include "segame.h"
#include "platform.h"
#include "resources.h"

#define PLAYER_TURN_SPEED real_from_float(2)
#define PLAYER_WALK_SPEED real_from_float(2) // might be too fast for release, but for testing good
#define PLAYER_MAX_STEP_HEIGHT 7

void player_resetMovement(Player* me)
{
    me->isTurningLeft = me->isTurningRight = me->isWalking = false;
}

void player_reset(Player* me)
{
    me->maxHealth = 2;
    me->health = 2;
    me->gold = 15;
    me->activeAction = PLAYERACT_USE;
    player_resetMovement(me);
}

void player_update_movement(Player* me, Renderer* renderer)
{
    if (me->isTurningLeft)
        renderer_rotate(renderer, real_neg(PLAYER_TURN_SPEED));
    if (me->isTurningRight)
        renderer_rotate(renderer, PLAYER_TURN_SPEED);
    if (me->isWalking)
        renderer_walk(renderer, (xz_t) { .x = real_zero, .z = PLAYER_WALK_SPEED }, PLAYER_MAX_STEP_HEIGHT);  
}

void segame_main_menu(SEGame* me, int button)
{
    UNUSED(me);
    APP_LOG(APP_LOG_LEVEL_ERROR, "Pressed %d", button);
}

SEGame* segame_init(SEGame* me, Renderer* renderer)
{
    memset(me, 0, sizeof(SEGame));
    player_reset(&me->player);
    me->renderer = renderer;
    me->player.location = renderer_getLocation(me->renderer);
    me->menu.game = me;
    me->isPaused = false;
    me->hadRenderedBefore = false;

    TextureManagerHandle textureManager = renderer_getTextureManager(renderer);
    me->iconDigits = sprite_load(textureManager, RESOURCE_ID_ICON_DIGITS);
    me->iconGold = sprite_load(textureManager, RESOURCE_ID_ICON_GOLD);
    me->iconHeart = sprite_load(textureManager, RESOURCE_ID_SPR_HEART);
    me->iconPlayerActions[PLAYERACT_WALK] = sprite_load(textureManager, RESOURCE_ID_ICON_BOOTS);
    me->iconPlayerActions[PLAYERACT_USE] = sprite_load(textureManager, RESOURCE_ID_ICON_KEY);
    me->iconPlayerActions[PLAYERACT_PICK] = sprite_load(textureManager, RESOURCE_ID_ICON_KEY);
    me->iconPlayerActions[PLAYERACT_BATTERY] = sprite_load(textureManager, RESOURCE_ID_ICON_KEY);
    me->iconPlayerActions[PLAYERACT_KEY] = sprite_load(textureManager, RESOURCE_ID_ICON_KEY);

    menu_reset(&me->menu);
    me->menu.text = "Want to skidaddle?";
    me->menu.buttons[0] = "> Yeppers <";
    me->menu.buttons[1] = "> NOPE <";
    me->menu.callback = segame_main_menu;

    return me;
}

void segame_free(SEGame* me)
{
    TextureManagerHandle textureManager = renderer_getTextureManager(me->renderer);
    sprite_free(textureManager, me->iconDigits);
    sprite_free(textureManager, me->iconGold);
    sprite_free(textureManager, me->iconHeart);
    for (int i = 0; i < COUNT_PLAYERACT; i++)
        sprite_free(textureManager, me->iconPlayerActions[i]);
}

void segame_update(SEGame* me)
{
    if (me->isPaused)
    {

    }
    else
    {
        player_update_movement(&me->player, me->renderer);
    }
}

void segame_input_select_click(SEGame* me)
{
    if (me->isPaused)
    {
        menu_select(&me->menu);
    }
    else
        me->player.isWalking = !me->player.isWalking;
}

void segame_input_direction_click(SEGame* me, bool isRight)
{
    if (!me->isPaused)
        return;
    if (isRight)
        menu_right(&me->menu);
    else
        menu_left(&me->menu);
}

void segame_input_direction_raw(SEGame* me, bool isRight, bool isDown)
{
    if (me->isPaused)
        return;
    if (isRight)
        me->player.isTurningRight = isDown;
    else
        me->player.isTurningLeft = isDown;
}

void segame_input_back_click(SEGame* me)
{
    if (me->isPaused)
    {
        menu_back(&me->menu);
    }
    else
    {
        // open main menu here
    }
}
