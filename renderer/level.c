#include <pebble.h>
#include "platform.h"
#include "level.h"

Level *level_load(int levelId)
{
    UNUSED(levelId);
    Wall walls_template[] = {
        {.startCorner = xz(real_from_int(70), real_from_int(50)),
         .color = GColorFromRGB(0, 255, 0)},
        {.startCorner = xz(real_from_int(0), real_from_int(50)),
         .color = GColorFromRGB(255, 0, 255)},
        {.startCorner = xz(real_from_int(0), real_from_int(-30)),
         .color = GColorFromRGB(0, 255, 255)}
    };
    Sector sectors_template[] = {
        {.wallCount = 3,
         .height = 25,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(0, 0, 255),
         .ceilColor = GColorFromRGB(255, 0, 0),
         .walls = NULL}
    };
    Level level_template = {
        .sectorCount = 1,
        .playerStart = {
            .sector = 0,
            .position = xz(real_from_int(20), real_from_int(20)),
            .angle = real_degToRad(real_from_int(0)),
            .height = real_zero
        },
        .sectors = NULL
    };

    char* memory = (char*)malloc(sizeof(Level) + sizeof(sectors_template) + sizeof(walls_template));
    if (memory == NULL)
        return NULL;
    Level* level = (Level*)memory;
    Sector* sectors = (Sector*)(memory + sizeof(Level));
    Wall* walls = (Wall*)(memory + sizeof(Level) + sizeof(sectors_template));
    memcpy(level, &level_template, sizeof(Level));
    memcpy(sectors, sectors_template, sizeof(sectors_template));
    memcpy(walls, walls_template, sizeof(walls_template));
    level->sectors = sectors;
    sectors->walls = walls;
    return level;
}

void level_free(Level *me)
{
    if (me == NULL)
        return;
    free(me);
}
