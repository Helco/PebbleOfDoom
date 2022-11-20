#include "segame.h"
#include "platform.h"
#include "resources.h"

static const char* const BookTexts[] = {
    "This is a book.\nCongratulations"
};

void book_init(SEGame* game, EntityData* data)
{
    UNUSED(game);
    data->actionDistanceSqr = real_from_int(50 * 50);
    data->playerAction = PLAYERACT_USE;
    data->entity->sprite = RESOURCE_ID_SPR_BOOK;
}

void book_act(SEGame* game, EntityData* data)
{
    UNUSED(game);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "book activated: ", data->entity->arg1);

    menu_reset(&game->menu);
    game->menu.text = BookTexts[data->entity->arg1];
    game->menu.callback = menu_cb_just_close;
}

#define START_ATTACK_TIMER 37
#define REPEAT_ATTACK_TIMER 30

void monster_init(SEGame* game, EntityData* data)
{
    TextureManagerHandle textureManager = renderer_getTextureManager(game->renderer);
    data->actionDistanceSqr = real_from_int(50*50);
    data->playerAction = game->player.hasBattery ? PLAYERACT_BATTERY : PLAYERACT_PICK;
    data->monster.distanceLeft = real_zero;
    data->monster.pushTimer = 0;
    data->monster.attackTimer = START_ATTACK_TIMER;
    if (data->entity->arg1 == 0)
    {
        data->monster.sprAlive = sprite_load(textureManager, RESOURCE_ID_SPR_PC);
        data->monster.sprPushed = sprite_load(textureManager, RESOURCE_ID_SPR_PC_PUSH);
        data->monster.sprDead = sprite_load(textureManager, RESOURCE_ID_SPR_PC_DEAD);
        data->monster.speed = real_from_float(1.3f);
        data->monster.health = 3;
    }
    else
    {
        data->monster.sprAlive = sprite_load(textureManager, RESOURCE_ID_SPR_PTR);
        data->monster.sprPushed = sprite_load(textureManager, RESOURCE_ID_SPR_PTR_PUSH);
        data->monster.sprDead = sprite_load(textureManager, RESOURCE_ID_SPR_PTR_DEAD);
        data->monster.speed = real_from_float(0.7f);
        data->monster.health = 5;
    }
    data->monster.regularSpeed = data->monster.speed;

    data->entity->sprite = data->monster.sprAlive->id;
}

void monster_pushMovement(SEGame* game, EntityData* data, int distance)
{
    xz_t dir = xz_sub(data->entity->location.position, game->player.location->position);
    data->monster.direction = xz_normalize(dir);
    data->monster.distanceLeft = real_from_int(distance);
    data->monster.speed = real_mul(data->monster.regularSpeed, real_from_int(10));
}

void monster_act(SEGame* game, EntityData* data)
{
    if (data->monster.pushTimer > 0 || data->monster.health < 0)
        return;

    trigger_haptic(50);
    data->monster.health -= game->player.hasBattery ? 3 : 1;
    if (data->monster.health > 0)
    {
        data->entity->sprite = data->monster.sprPushed->id;
        data->monster.pushTimer = 60;

        monster_pushMovement(game, data, data->entity->arg1 == 0 ? 150 : 100);
    }
    else
    {
        game->player.gold += data->entity->arg1 == 0 ? 2 : 5;
        data->entity->sprite = data->monster.sprDead->id;
        data->monster.health = 0;
        data->playerAction = PLAYERACT_WALK;
        monster_pushMovement(game, data, 50);
    }
}

bool monster_walk(SEGame* game, EntityData* data)
{
    real_t move = real_min(data->monster.distanceLeft, data->monster.speed);
    Location newLocation = data->entity->location;
    newLocation.position = xz_add(newLocation.position, xz_scale(data->monster.direction, move));
    if (!location_updateSectorNear(&newLocation, game->level))
    {
        data->entity->location = newLocation;
        data->monster.distanceLeft = real_sub(data->monster.distanceLeft, move);
        return true;
    }
    return false;
}

void monster_update(SEGame* game, EntityData* data)
{
    if (data->monster.health <= 0)
    {
        monster_walk(game, data);
        data->monster.health--;
        if (data->monster.health > -20)
            return;
        else if (data->monster.health < -40)
        {
            data->isDead = true;
            data->entity->sprite = INVALID_SPRITE_ID;
        }
        int sectorHeight = game->level->sectors[data->entity->location.sector].heightOffset;
        data->entity->location.height = real_sub(
            real_from_int(sectorHeight + 5),
            real_lerpi(-data->monster.health, 30, 45, 0, 15));

        int jitterFreqDiv = data->entity->arg1 == 0 ? 1 : 2;
        int jitterAmplDiv = data->entity->arg1 == 0 ? 2 : 1;
        real_t jitter = real_div(real_from_int((-data->monster.health / jitterFreqDiv % 2) * 2 - 1), real_from_int(jitterAmplDiv));
        data->entity->location.position = xz_add(data->entity->location.position, xz(jitter, jitter));
    }
    else if (data->monster.pushTimer > 0)
    {
        monster_walk(game, data);
        data->monster.speed = real_lerp(real_from_float(0.2f), data->monster.speed, data->monster.regularSpeed);
        data->monster.pushTimer--;
        if (data->monster.pushTimer <= 0)
        {
            data->monster.pushTimer = 0;
            data->entity->sprite = data->monster.sprAlive->id;
            data->monster.attackTimer = START_ATTACK_TIMER;
        }
    }
    else if (game->player.location->sector == data->entity->location.sector)
    {
        data->monster.wasAttacking = true;
        if (real_compare(data->curDistanceSqr, real_from_int(25 * 25)) > 0)
        {
            xz_t dir = xz_sub(game->player.location->position, data->entity->location.position);
            data->monster.direction = xz_normalize(dir);
            data->monster.distanceLeft = real_from_int(100000);
            monster_walk(game, data);
            data->monster.attackTimer = min(data->monster.attackTimer + 2, START_ATTACK_TIMER);
        }
        else if (--data->monster.attackTimer <= 0)
        {
            data->monster.attackTimer = REPEAT_ATTACK_TIMER;
            if (rand() % 3 > 0)
            {
                segame_hurtPlayer(game);
            }
        }
    }
    else
    {
        data->monster.speed = real_lerp(real_from_float(0.05f), data->monster.speed,
            real_div(data->monster.regularSpeed, real_from_int(2)));
        data->monster.attackTimer = min(data->monster.attackTimer + 1, START_ATTACK_TIMER);

        if (data->monster.wasAttacking || real_compare(data->monster.distanceLeft, real_zero) <= 0)
        {
            data->monster.direction = xz_rotate(xz_one, real_degToRad(real_from_int(rand() % 360)));
            data->monster.distanceLeft = real_from_int(50 + (rand() % 400));
        }
        if (!monster_walk(game, data))
        {
            data->monster.distanceLeft = real_zero;
        }
    }
}

void monster_dtor(SEGame* game, EntityData* data)
{
    TextureManagerHandle textureManager = renderer_getTextureManager(game->renderer);
    sprite_free(textureManager, data->monster.sprAlive);
    sprite_free(textureManager, data->monster.sprPushed);
    sprite_free(textureManager, data->monster.sprDead);
}
