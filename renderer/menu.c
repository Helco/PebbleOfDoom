#include "segame.h"
#include "platform.h"

void menu_open(Menu* menu)
{
    menu->hadBeenRendered = false;
    menu->size = (GSize){ .w = 0, .h = 0 };
    menu->curButton = 0;
    menu->flippedButton = -1;
    menu->game->isPaused = true;
}

void menu_left(Menu* menu)
{
    menu->curButton--;
    if (menu->curButton < 0)
    {
        for (int i = MAX_BUTTONS - 1; i >= 0; i--)
        {
            if (menu->buttons[i] != NULL)
            {
                menu->curButton = i;
                return;
            }
        }
    }
}

void menu_right(Menu* menu)
{
    menu->curButton++;
    if (menu->curButton >= MAX_BUTTONS || menu->buttons[menu->curButton] == NULL)
        menu->curButton = 0;
}

void menu_select(Menu* menu)
{
    assert(menu->callback != NULL);
    if (menu->buttons[0] == NULL)
        menu->callback(menu->game, -1);
    else
    {
        assert(menu->curButton >= 0 && menu->curButton < MAX_BUTTONS &&
            menu->buttons[menu->curButton] != NULL);
        menu->callback(menu->game, menu->curButton);
    }
}
