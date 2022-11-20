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
