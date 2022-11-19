#ifndef SEGAME_H
#define SEGAME_H
#include "renderer.h"

typedef int PlayerAction;
enum PlayerAction_ {
    PLAYERACT_WALK,
    PLAYERACT_USE,
    PLAYERACT_PICK,
    PLAYERACT_BATTERY,
    PLAYERACT_KEY,
    COUNT_PLAYERACT
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
    bool hadRenderedBefore;

    const Sprite* iconDigits;
    const Sprite* iconPlayerActions[COUNT_PLAYERACT];
    const Sprite* iconGold;
    const Sprite* iconHeart;
} SEGame;

SEGame* segame_init(SEGame* me, Renderer* renderer);
void segame_free(SEGame* me);
void segame_update(SEGame* me);
void segame_render(SEGame* me, RendererTarget renderer);

void segame_input_select_click(SEGame* me);
void segame_input_back_click(SEGame* me);
void segame_input_direction_click(SEGame* me, bool isRight);
void segame_input_direction_raw(SEGame* me, bool isRight, bool isDown);

#endif
