#include <pebble.h>
#include "platform.h"
#include "level.h"

Level *level_load(int levelId)
{
    UNUSED(levelId);
    Wall walls_template[] = {
        // start triangle
        {.startCorner = xz(real_from_int(70), real_from_int(50)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = 1},
        {.startCorner = xz(real_from_int(0), real_from_int(50)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},
        {.startCorner = xz(real_from_int(0), real_from_int(-30)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},

        // northern trapez
        {.startCorner = xz(real_from_int(70), real_from_int(50)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},//{ xy_zero, xy_one},
         .portalTo = 2},
        {.startCorner = xz(real_from_int(50), real_from_int(80)),
         .texture = 0,
         .texCoord = { { real_zero, real_one }, { real_one, real_zero } },
         .portalTo = -1},
        {.startCorner = xz(real_from_int(20), real_from_int(80)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = 3},
        {.startCorner = xz(real_from_int(00), real_from_int(50)),
         .texture = 0,
         .texCoord = { { real_zero, real_one }, { real_one, real_zero } },
         .portalTo = 0},

        // right wing
        {.startCorner = xz(real_from_int(70), real_from_int(50)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},
        {.startCorner = xz(real_from_int(90), real_from_int(65)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},
        {.startCorner = xz(real_from_int(50), real_from_int(80)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = 1},

        // left wing
        {.startCorner = xz(real_from_int(00), real_from_int(50)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = 1},
        {.startCorner = xz(real_from_int(20), real_from_int(80)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},
        {.startCorner = xz(real_from_int(-20), real_from_int(65)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one},
         .portalTo = -1},
    };
    Sector sectors_template[] = {
        {.wallCount = 3,
         .height = 25,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(0, 0, 255),
         .ceilColor = GColorFromRGB(255, 0, 0),
         .walls = NULL},
        {.wallCount = 4,
         .height = 35,
         .heightOffset = -5,
         .floorColor = GColorFromRGB(0, 128, 255),
         .ceilColor = GColorFromRGB(128, 128, 255),
         .walls = NULL},
        {.wallCount = 3,
         .height = 25,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(255, 255, 128),
         .ceilColor = GColorFromRGB(255, 255, 0),
         .walls = NULL},
        {.wallCount = 3,
         .height = 25,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(0, 255, 128),
         .ceilColor = GColorFromRGB(0, 255, 128),
         .walls = NULL}
    };
    Level level_template = {
        .sectorCount = 4,
        .playerStart = {
            .sector = 1,
            .position = xz(real_from_int(20), real_from_int(70)),
            .angle = real_degToRad(real_from_int(40)),
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
    sectors[0].walls = walls;
    sectors[1].walls = walls + 3;
    sectors[2].walls = walls + 3 + 4;
    sectors[3].walls = walls + 3 + 4 + 3;
    return level;
}

void level_free(Level *me)
{
    if (me == NULL)
        return;
    free(me);
}
