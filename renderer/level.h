#ifndef LEVEL_H
#define LEVEL_H

#include "algebra.h"
#include "texture.h"
#include <stdbool.h>

#define INVALID_LEVEL_ID ((LevelId)-1)

typedef int LevelId;
typedef void* LevelManagerHandle;
typedef uint8_t WallFlags;

enum WallFlags_ {
    WALL_CONTOUR_LEFT           = (1 << 0),
    WALL_CONTOUR_LEFTPORTAL     = (1 << 1),
    WALL_CONTOUR_RIGHT          = (1 << 2),
    WALL_CONTOUR_RIGHTPORTAL    = (1 << 3),
    WALL_CONTOUR_TOP            = (1 << 4),
    WALL_CONTOUR_TOPPORTAL      = (1 << 5),
    WALL_CONTOUR_BOTTOM         = (1 << 6),
    WALL_CONTOUR_BOTTOMPORTAL   = (1 << 7)
};

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
    GColor color;
    WallFlags flags;
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
