#ifndef SEGAME_H
#define SEGAME_H
#include "renderer.h"

#define MENU_BORDER 4
#define MENU_BUTTON_SPACE 16
#define MAX_BUTTONS 4

typedef int PlayerAction;
enum PlayerAction_ {
    PLAYERACT_WALK,
    PLAYERACT_USE,
    PLAYERACT_PICK,
    PLAYERACT_BATTERY,
    PLAYERACT_KEY,
    COUNT_PLAYERACT
};

typedef struct SEGame SEGame;
typedef void (*MenuCallback)(SEGame* game, int button);

typedef struct Player
{
    Location* location;
    int health, maxHealth; // in half hearts
    int gold;
    PlayerAction activeAction;
    bool isWalking, isTurningLeft, isTurningRight;
} Player;

typedef struct Menu
{
    SEGame* game;
    GSize size;
    bool hadBeenRendered;
    const char* text;
    const char* buttons[MAX_BUTTONS];
    GRect buttonRects[MAX_BUTTONS];
    int curButton, flippedButton;
    MenuCallback callback;
} Menu;

struct SEGame
{
    Renderer* renderer;
    Player player;
    Menu menu;
    bool isPaused;
    bool hadRenderedBefore;

    const Sprite* iconDigits;
    const Sprite* iconPlayerActions[COUNT_PLAYERACT];
    const Sprite* iconGold;
    const Sprite* iconHeart;
};

SEGame* segame_init(SEGame* me, Renderer* renderer);
void segame_free(SEGame* me);
void segame_update(SEGame* me);
void segame_render(SEGame* me, RendererTarget renderer);

void menu_reset(Menu* menu);
void menu_render(Menu* menu, RendererTarget renderer);
void menu_left(Menu* menu);
void menu_right(Menu* menu);
void menu_select(Menu* menu);
void menu_back(Menu* menu);

void segame_input_select_click(SEGame* me);
void segame_input_back_click(SEGame* me);
void segame_input_direction_click(SEGame* me, bool isRight);
void segame_input_direction_raw(SEGame* me, bool isRight, bool isDown);

#endif
