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
    data->playerAction = game->player.hasBattery ? PLAYERACT_BATTERY : PLAYERACT_FIST;
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
        data->monster.pushTimer = 30;

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

void techpriest_menu_heal(SEGame* game, int button)
{
    UNUSED(button);
    game->player.health = game->player.maxHealth;
    menu_reset(&game->menu);
    game->menu.text = "Here you go,\nlike new!";
    game->menu.callback = menu_cb_just_close;
}

static const char* const techpriest_lines_new[] =
{
    "Hello my child and welcome!",
    "I am the priest of technology and guard of this holy place.",
    "You look like you could help me: it was long ago since I knew the time...",
    "Once there were many watches here but alas they did not last and thus were banished into the CAVES below",
    "There is said to be a special one, a RARE watch to outbest them",
    "Would you try to find it? But be careful, the OLD ONES are filled with anger!",
    NULL
};

static const char* const techpriest_lines_notentered[] =
{
    "Yes this is the KEY TO THE CAVES, but what are you waiting for?",
    "Only the caves could hold the secret, you are wasting your time here!",
    NULL
};

static const char* const techpriest_lines_noprogress[] =
{
    "You have to go deeper, the clue is bound to be there!",
    "Do you call this a wound? The old ones have not even touched you!",
    "Are you a watch? No?! THEN STOP WASTING MY TIME!"
};

static const char* const techpriest_lines_wounded[] =
{
    "Oh yes, the old ones gotten to you. Let me see...",
    "You know, I was a doctor once. Somewhere around the last century...",
    "Are you even trying to be careful?"
};

static const char* const techpriest_lines_battery[] =
{
    "Of course a battery defeats the old ones easily, it even revives them.",
    "What were you doing before? Just PUNCHING them?!",
    NULL
};

static const char* const techpriest_lines_gotclue[] =
{
    "What have you there, an old diary? Does it reveal anything?",
    "A code? But there is only one lock of that type in this village...",
    "GIVE IT TO ME! You will not get EMERY, you are not WORTHY!",
    NULL
};

void techpriest_init(SEGame* game, EntityData* data)
{
    UNUSED(game);
    data->actionDistanceSqr = real_from_int(30 * 30);
    data->playerAction = PLAYERACT_SPEAK;
    data->entity->sprite = RESOURCE_ID_SPR_TECHPRIEST;
    data->techpriest.noProgressLine = 0;
}

void techpriest_act(SEGame* game, EntityData* data)
{
    if (!game->player.hasSpokenToPriest)
    {
        game->menu.lines = techpriest_lines_new;
        game->menu.lineI = -1;
        menu_cb_babble_lines(game, -1);
        game->player.hasSpokenToPriest = true;
        game->player.gold += 5;
    }
    else if (!game->player.hasKey)
    {
        menu_reset(&game->menu);
        game->menu.text = "The greed SHOPKEEPER only sells the KEY TO THE CAVES anymore. What a ...";
        game->menu.callback = menu_cb_just_close;
    }
    else if (!game->player.hasEnteredCave)
    {
        game->menu.lines = techpriest_lines_notentered;
        game->menu.lineI = -1;
        menu_cb_babble_lines(game, -1);
    }
    else if (game->player.hasGotClue)
    {
        game->menu.lines = techpriest_lines_gotclue;
        game->menu.lineI = -1;
        menu_cb_babble_lines(game, -1);
        // TODO: Add techpriest monster here
    }
    else if (game->player.health < game->player.maxHealth)
    {
        menu_reset(&game->menu);
        game->menu.text = techpriest_lines_wounded[rand() % 3];
        game->menu.callback = techpriest_menu_heal;
    }
    else if (!game->player.priestHasSeenBattery)
    {
        game->menu.lines = techpriest_lines_battery;
        game->menu.lineI = -1;
        menu_cb_babble_lines(game, -1);
        game->player.priestHasSeenBattery = true;
    }
    else //if (game->player.health == game->player.maxHealth)
    {
        menu_reset(&game->menu);
        game->menu.text = techpriest_lines_noprogress[data->techpriest.noProgressLine];
        game->menu.callback = menu_cb_just_close;
        data->techpriest.noProgressLine = (data->techpriest.noProgressLine + 1) % 3;
    }
}

const char* const shopkeeper_lines_new[] =
{
    "Well hello there, and who are you?",
    "I am the shopkeeper, I am the only one selling things around here",
    "Do you want into the caves? Only I have the KEY TO THE CAVES",
    "And you can buy it. Here. If you have the gold. Three gold.",
    NULL
};

const char* const shopkeeper_lines_random[] =
{
    "Well, are you already back? Too scared of the caves?",
    "Don't just stand there, buy something if you are already here.",
    "Has the priest also send you on the quest? You are just like the rest, hehehe..."
};

void shopkeeper_init(SEGame* game, EntityData* data)
{
    UNUSED(game);
    data->actionDistanceSqr = real_from_int(30 * 30);
    data->playerAction = PLAYERACT_SPEAK;
    data->entity->sprite = RESOURCE_ID_SPR_SHOPKEEPER;
    data->entity->radius = 10;
}

void shopkeeper_act(SEGame* game, EntityData* data)
{
    UNUSED(data);
    if (!game->player.hasSpokenToShopKeeper)
    {
        game->player.hasSpokenToShopKeeper = true;
        game->menu.lines = shopkeeper_lines_new;
        game->menu.lineI = -1;
    }
    else if (!game->player.hasSpokenToPriest)
    {
        menu_reset(&game->menu);
        game->menu.text = "Well I don't have any work for you, but maybe the priest in the cathedral?";
        game->menu.callback = menu_cb_just_close;
    }
    else if (!game->player.hasKey && !game->player.hasEnteredCave)
    {
        menu_reset(&game->menu);
        game->menu.text = "Sure you can buy the key, here you go.\nHave fun, don't let the grumpy watches bite you";
        game->menu.callback = menu_cb_just_close;
        game->player.hasKey = true;
        game->player.gold -= 3;
    }
    else if (game->player.hasKey && !game->player.hasEnteredCave)
    {
        menu_reset(&game->menu);
        game->menu.text = "Well go on if you will. And if you don't make it, could you return the key?";
        game->menu.callback = menu_cb_just_close;
    }
    else
    {
        menu_reset(&game->menu);
        game->menu.text = shopkeeper_lines_random[rand() % 3];
        game->menu.callback = menu_cb_just_close;
    }
}

void itemoffer_init(SEGame* game, EntityData* data)
{
    UNUSED(game);
    data->actionDistanceSqr = real_from_int(30 * 30);
    data->playerAction = PLAYERACT_SPEAK;
    data->entity->sprite = data->entity->arg1 == 0 ? RESOURCE_ID_SPR_BATTERY : RESOURCE_ID_SPR_HEART;
    data->entity->radius = 12;
}

static int activatedItem = 0;

void itemoffer_menu_result(SEGame* game, int button)
{
    int price = activatedItem == 0 ? 10 : 5;
    if (button == 0 && game->player.gold >= price)
    {
        game->player.gold -= price;
        menu_reset(&game->menu);
        game->menu.text = "Here you go.";
        game->menu.callback = menu_cb_just_close;
    }
    else if (button == 0 && game->player.gold < price)
    {
        menu_reset(&game->menu);
        game->menu.text = "You don't have enough gold. Come back when you are less poor";
        game->menu.callback = menu_cb_just_close;
    }
    else if (button == 1)
    {
        game->isPaused = false;
    }
}

void itemoffer_menu_confirm(SEGame* game, int button)
{
    UNUSED(button);
    menu_reset(&game->menu);
    game->menu.text = activatedItem == 0
        ? "Want to buy the battery for\n10 Gold?"
        : "Want to buy the Heart for\n5 Gold?";
    game->menu.buttons[0] = "Yes";
    game->menu.buttons[1] = "No";
    game->menu.callback = itemoffer_menu_result;
}

void itemoffer_act(SEGame* game, EntityData* data)
{
    activatedItem = data->entity->arg1;
    menu_reset(&game->menu);
    game->menu.text = activatedItem == 0
        ? "A battery.\nGood against all machines that have just run out of juice."
        : "A heart.\nIf one is good, why not have two?";
    game->menu.callback = itemoffer_menu_confirm;
}

void door_init(SEGame* game, EntityData* data)
{
    UNUSED(game);
    data->playerAction = PLAYERACT_DOOR;
    data->actionDistanceSqr = real_from_int(20 * 20);
    data->entity->sprite = INVALID_SPRITE_ID;
}

static const LevelId DoorTargets[] = {
    RESOURCE_ID_LVL_HOME,
    RESOURCE_ID_LVL_OVERWORLD,
    RESOURCE_ID_LVL_SHOP,
    RESOURCE_ID_LVL_CATHEDRAL,
    RESOURCE_ID_LVL_CAVE
};

void door_act(SEGame* game, EntityData* data)
{
    if (data->entity->arg3 > 0 && !game->player.hasKey)
    {
        menu_reset(&game->menu);
        game->menu.text = "The door is locked.";
        game->menu.callback = menu_cb_just_close;
        return;
    }

    int targetEntry = data->entity->arg2 >> 4;
    segame_changeLevel(game, DoorTargets[data->entity->arg1]);

    for (int i = 0; i < game->level->sectorCount; i++)
    {
        const Sector* sector = &game->level->sectors[i];
        for (int j = 0; j < sector->entityCount; j++)
        {
            const Entity* entity = &sector->entities[j];
            if (entity->type == ENTITY_DOOR && (entity->arg2 & 0xf) == targetEntry)
            {
                *game->player.location = entity->location;
                return;
            }
        }
    }

    assert(false && "Invalid target entry");
}
