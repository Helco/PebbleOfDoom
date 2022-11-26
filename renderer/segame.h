#ifndef SEGAME_H
#define SEGAME_H
#include "renderer.h"

#define MENU_BORDER 4
#define MENU_BUTTON_SPACE 16
#define MAX_BUTTONS 4
#define MAX_ENTITIES 16
#define MAX_FOCUS_ANGLE 0.35f
#define MAX_STOP_DISTANCE real_from_int(35 * 35)

typedef int PlayerAction;
enum PlayerAction_ {
    PLAYERACT_WALK,
    PLAYERACT_USE,
    PLAYERACT_FIST,
    PLAYERACT_BATTERY,
    PLAYERACT_KEY,
    PLAYERACT_SPEAK,
    PLAYERACT_DOOR,
    COUNT_PLAYERACT
};

typedef enum EntityType_ {
    ENTITY_BOOK,
    ENTITY_ITEM_OFFER,
    ENTITY_MONSTER,
    ENTITY_SHOPKEEPER,
    ENTITY_TECHPRIEST,
    ENTITY_DOOR,
    ENTITY_KEY,
    ENTITY_SAFE,
    ENTITY_EMERY
} EntityType;

typedef struct SEGame SEGame;
typedef void (*MenuCallback)(SEGame* game, int button);

typedef struct Player
{
    Location* location;
    int health, maxHealth; // in half hearts
    int gold;
    PlayerAction activeAction;
    bool isWalking, isTurningLeft, isTurningRight;
    bool hasHomeKey, hasMineKey, hasCathKey, hasBattery, hasEnteredCave, hasSpokenToPriest, hasGotClue;
    bool priestHasSeenBattery, hasSpokenToShopKeeper, shopKeeperTold;
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
    MenuCallback babbleCallback;

    int lineI;
    const char* const* lines;
} Menu;


typedef struct EntityData EntityData;
typedef void (*EntityCallback)(SEGame* game, EntityData* data);

typedef struct EntityBrain
{
    EntityCallback init;
    EntityCallback playerAction;
    EntityCallback update;
    EntityCallback dtor;
} EntityBrain;

struct EntityData
{
    const EntityBrain* brain;
    Entity* entity;
    PlayerAction playerAction;
    real_t actionDistanceSqr;
    real_t curDistanceSqr;
    bool isDead;
    union {
        struct {
            const char* text;
        } book;
        struct {
            const char* text;
            int price;
        } offer;
        struct {
            xz_t direction;
            real_t distanceLeft;
            real_t speed;
            real_t regularSpeed;
            int health;
            int pushTimer;
            int attackTimer;
            bool wasAttacking;

            const Sprite* sprAlive, * sprPushed, * sprDead;
        } monster;
        struct {
        } shopkeeper;
        struct {
            int noProgressLine;
        } techpriest;
        struct {
            LevelId level;
        } door;
    };
};

typedef void (*SEGameOnceCallback)(SEGame* game);

struct SEGame
{
    Renderer* renderer;
    LevelManagerHandle levelManager;
    const Level* level;
    Player player;
    Menu menu;
    bool isPaused;
    bool hadRenderedBefore;
    bool gameIsRunning;
    SEGameOnceCallback onceCallback;

    EntityData* focusedEntity;
    EntityData entities[MAX_ENTITIES];
    int entityCount;

    const Sprite* iconDigits;
    const Sprite* iconPlayerActions[COUNT_PLAYERACT];
    const Sprite* iconGold;
    const Sprite* iconHeart;
};

SEGame* segame_init(SEGame* me, Renderer* renderer, LevelManagerHandle levelManager);
void segame_end(SEGame* me);
void segame_mainmenu(SEGame* me);
void segame_free(SEGame* me);
void segame_update(SEGame* me);
void segame_render(SEGame* me, RendererTarget renderer);
void segame_changeLevel(SEGame* me, LevelId newLevel);
void segame_hurtPlayer(SEGame* me);
void segame_once_tutorial(SEGame* me);
void segame_once_died(SEGame* me);
void segame_once_angered(SEGame* me);

void menu_reset(Menu* menu);
void menu_render(Menu* menu, RendererTarget renderer);
void menu_left(Menu* menu);
void menu_right(Menu* menu);
void menu_select(Menu* menu);
void menu_back(Menu* menu);
void menu_cb_just_close(SEGame* game, int button);
void menu_cb_babble_lines(SEGame* game, int button);

void book_init(SEGame* game, EntityData* data);
void book_act(SEGame* game, EntityData* data);

void monster_init(SEGame* game, EntityData* data);
void monster_act(SEGame* game, EntityData* data);
void monster_update(SEGame* game, EntityData* data);
void monster_dtor(SEGame* game, EntityData* data);

void techpriest_init(SEGame* game, EntityData* data);
void techpriest_act(SEGame* game, EntityData* data);

void shopkeeper_init(SEGame* game, EntityData* data);
void shopkeeper_act(SEGame* game, EntityData* data);

void itemoffer_init(SEGame* game, EntityData* data);
void itemoffer_act(SEGame* game, EntityData* data);

void door_init(SEGame* game, EntityData* data);
void door_act(SEGame* game, EntityData* data);
void door_update(SEGame* game, EntityData* data);

void key_init(SEGame* game, EntityData* data);
void key_act(SEGame* game, EntityData* data);

void safe_init(SEGame* game, EntityData* data);
void safe_act(SEGame* game, EntityData* data);

void emery_init(SEGame* game, EntityData* data);
void emery_act(SEGame* game, EntityData* data);

void segame_input_select_click(SEGame* me);
void segame_input_select_long_click(SEGame* me);
void segame_input_back_click(SEGame* me);
void segame_input_direction_click(SEGame* me, bool isRight);
void segame_input_direction_raw(SEGame* me, bool isRight, bool isDown);

#endif
