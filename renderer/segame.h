#ifndef SEGAME_H
#define SEGAME_H
#include "renderer.h"

typedef int PlayerAction;
enum PlayerAction_ {
    PLAYERACT_WALK,
    PLAYERACT_USE,
    PLAYERACT_PICK,
    PLAYERACT_BATTERY,
    PLAYERACT_KEY
};

typedef struct Player
{
    Location* location;
    int health, maxHealth; // in half hearts
    int gold;
    PlayerAction activeAction;
    bool isWalking, isTurningLeft, isTurningRight;
} Player;

typedef struct SEGame
{
    Renderer* renderer;
    Player player;
    bool isPaused;
} SEGame;

SEGame* segame_init(SEGame* me, Renderer* renderer);
void segame_free(SEGame* me);
void segame_update(SEGame* me);

void segame_input_select_click(SEGame* me);
void segame_input_back_click(SEGame* me);
void segame_input_direction_click(SEGame* me, bool isRight);
void segame_input_direction_raw(SEGame* me, bool isRight, bool isDown);

#endif
