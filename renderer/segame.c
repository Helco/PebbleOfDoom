#include "segame.h"
#include "platform.h"
#include "resources.h"

#define PLAYER_TURN_SPEED real_from_float(3)
#define PLAYER_WALK_SPEED real_from_float(2) // might be too fast for release, but for testing good
#define PLAYER_MAX_STEP_HEIGHT 12

static const EntityBrain EntityBrains[] = {
    [ENTITY_BOOK] = {
        .init = book_init,
        .playerAction = book_act
    },
    [ENTITY_ITEM_OFFER] = {
        .init = itemoffer_init,
        .playerAction = itemoffer_act
    },
    [ENTITY_MONSTER] = {
        .init = monster_init,
        .playerAction = monster_act,
        .update = monster_update,
        .dtor = monster_dtor
    },
    [ENTITY_TECHPRIEST] = {
        .init = techpriest_init,
        .playerAction = techpriest_act
    },
    [ENTITY_SHOPKEEPER] = {
        .init = shopkeeper_init,
        .playerAction = shopkeeper_act
    }
};

void player_resetMovement(Player* me)
{
    me->isTurningLeft = me->isTurningRight = me->isWalking = false;
}

void player_reset(Player* me)
{
    me->maxHealth = 2;
    me->health = 2;
    me->gold = 0;
    me->activeAction = PLAYERACT_WALK;
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

SEGame* segame_init(SEGame* me, Renderer* renderer, LevelManagerHandle levelManager)
{
    memset(me, 0, sizeof(SEGame));
    player_reset(&me->player);
    me->renderer = renderer;
    me->levelManager = levelManager;
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
    me->iconPlayerActions[PLAYERACT_FIST] = sprite_load(textureManager, RESOURCE_ID_ICON_FIST);
    me->iconPlayerActions[PLAYERACT_BATTERY] = sprite_load(textureManager, RESOURCE_ID_ICON_BATTERY);
    me->iconPlayerActions[PLAYERACT_KEY] = sprite_load(textureManager, RESOURCE_ID_ICON_KEY);
    me->iconPlayerActions[PLAYERACT_SPEAK] = sprite_load(textureManager, RESOURCE_ID_ICON_SPEAK);

    segame_changeLevel(me, RESOURCE_ID_LVL_SHOP);
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
    segame_changeLevel(me, INVALID_LEVEL_ID);
}

void segame_update(SEGame* me)
{
    if (me->isPaused)
    {

    }
    else
    {
        player_update_movement(&me->player, me->renderer);

        me->focusedEntity = NULL;
        real_t lastDistanceSqr = real_from_int(10000);
        for (int i = 0; i < me->entityCount; i++)
        {
            EntityData* entity = &me->entities[i];
            if (entity->isDead)
                continue;

            xz_t playerToEntity = xz_sub(entity->entity->location.position, me->player.location->position);
            entity->curDistanceSqr = xz_lengthSqr(playerToEntity);

            if (entity->playerAction != PLAYERACT_WALK &&
                real_compare(entity->curDistanceSqr, entity->actionDistanceSqr) < 0 &&
                real_compare(entity->curDistanceSqr, lastDistanceSqr) < 0)
            {
                xz_t localDirection = xz_invScale(playerToEntity, real_sqrt(entity->curDistanceSqr));
                localDirection = xz_rotate(localDirection, me->player.location->angle);
                // yes. too cheap for an acos
                if (real_compare(real_abs(localDirection.x), real_from_float(MAX_FOCUS_ANGLE)) < 0 &&
                    real_compare(localDirection.z, real_zero) > 0)
                {
                    me->focusedEntity = entity;
                    lastDistanceSqr = entity->curDistanceSqr;
                    if (real_compare(entity->curDistanceSqr, MAX_STOP_DISTANCE) < 0)
                        me->player.isWalking = false;
                }
            }

            EntityCallback cb = entity->brain->update;
            if (cb != NULL)
                cb(me, entity);
        }

        me->player.activeAction = me->focusedEntity == NULL
            ? PLAYERACT_WALK
            : me->focusedEntity->playerAction;
    }
}

void segame_changeLevel(SEGame* me, LevelId levelId)
{
    if (me->level != NULL)
        level_free(me->levelManager, me->level);
    for (int i = 0; i < me->entityCount; i++)
    {
        if (me->entities[i].brain->dtor != NULL)
            me->entities[i].brain->dtor(me, &me->entities[i]);
    }

    if (levelId == INVALID_LEVEL_ID)
        return;

    me->level = level_load(me->levelManager, levelId);
    renderer_setLevel(me->renderer, me->level);

    int entityI = 0;
    for (int i = 0; i < me->level->sectorCount; i++)
    {
        for (int j = 0; j < me->level->sectors[i].entityCount; j++, entityI++)
        {
            Entity* entity = &me->level->sectors[i].entities[j];
            EntityData* data = &me->entities[entityI];
            data->entity = entity;
            data->isDead = false;
            data->brain = &EntityBrains[entity->type];
            data->brain->init(me, data);
        }
    }
    me->entityCount = entityI;
    me->focusedEntity = NULL;
}

void segame_menu_died_3(SEGame* me, int button)
{
    UNUSED(button);
    me->player.health = me->player.maxHealth;
    menu_reset(&me->menu);
    me->menu.text = "Let me patch you up. Now go and be more careful!";
    me->menu.callback = menu_cb_just_close;
}

void segame_menu_died_2(SEGame* me, int button)
{
    UNUSED(button);
    menu_reset(&me->menu);
    me->menu.text = "I found you outside the cave.\nSeems like someone stole your gold.";
    me->menu.callback = segame_menu_died_3;
}

void segame_hurtPlayer(SEGame* me)
{
    trigger_haptic(200);
    me->player.health--;
    if (me->player.health < 0)
    {
        me->player.health = 1;
        me->player.gold /= 3;
        segame_changeLevel(me, RESOURCE_ID_LVL_CATHEDRAL);
        //me->player.location->position = xz(real_from_int(300), real_from_int(200)); 
        menu_reset(&me->menu);
        me->menu.text = "Hey, wake up!\nOh good, you are alive...";
        me->menu.callback = segame_menu_died_2;
    }
}

void segame_input_select_click(SEGame* me)
{
    if (me->isPaused)
    {
        menu_select(&me->menu);
    }
    else if (me->player.isWalking || me->player.activeAction == PLAYERACT_WALK)
        me->player.isWalking = !me->player.isWalking;
}

void segame_input_select_long_click(SEGame* me)
{
    if (me->isPaused)
        return;
    if (me->focusedEntity != NULL && me->focusedEntity->brain->playerAction != NULL)
        me->focusedEntity->brain->playerAction(me, me->focusedEntity);
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
