#ifndef LEVEL_H
#define LEVEL_H

#include "algebra.h"
#include "texture.h"
#include <stdbool.h>

#define INVALID_LEVEL_ID ((LevelId)-1)

typedef int LevelId;
typedef void* LevelManagerHandle;

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
    int startCorner;
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
    int sectorCount, vertexCount;
    Location playerStart;
    Sector* sectors;
    xz_t* vertices;
} Level;

extern const Level* level_load(LevelManagerHandle lvlManager, LevelId id);
extern void level_free(LevelManagerHandle lvlManger, const Level* level);

int level_findSectorAt(const Level* level, xz_t point);
bool sector_isInside(const Sector* sector, const xz_t* vertices, xz_t point);
bool location_updateSector(Location* location, const Level* level); // returns true if sector was changed

#endif
