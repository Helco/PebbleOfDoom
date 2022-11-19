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
    me->health = me->maxHealth = 1;
    me->gold = 0;
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

SEGame* segame_init(SEGame* me, Renderer* renderer)
{
    player_reset(&me->player);
    me->renderer = renderer;
    me->player.location = renderer_getLocation(me->renderer);
    me->isPaused = false;
    return me;
}

void segame_free(SEGame* me)
{
    UNUSED(me);
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

    }
    else
        me->player.isWalking = !me->player.isWalking;
}

void segame_input_direction_click(SEGame* me, bool isRight)
{
    UNUSED(me, isRight);
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
    me->isPaused = !me->isPaused;
}
