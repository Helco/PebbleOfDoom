#ifndef LEVEL_H
#define LEVEL_H

#include "algebra.h"
#include <stdbool.h>

typedef int TextureId;

typedef struct TexCoord {
    xy_t start, end;
} TexCoord;

typedef struct Location {
    int sector;
    xz_t position;
    real_t height;
    real_t angle;
} Location;

typedef struct Wall {
    xz_t startCorner;
    int portalTo;
    TextureId texture;
    TexCoord texCoord;
} Wall;

typedef struct Sector {
    int wallCount;
    int height, heightOffset;
    GColor floorColor;
    GColor ceilColor;
    Wall* walls;
} Sector;

typedef struct Level {
    int sectorCount;
    Location playerStart;
    Sector* sectors;
} Level;

Level* level_load(int levelId);
void level_free(Level* me);
int level_findSectorAt(const Level* level, xz_t point);

bool sector_isInside(const Sector* sector, xz_t point);

// returns true if sector was changed
bool location_updateSector(Location* location, const Level* level);

#endif
