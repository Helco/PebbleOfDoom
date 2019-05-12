#ifndef LEVELSTORAGE_H
#define LEVELSTORAGE_H
#include "../renderer/platform.h"

#define LEVEL_STORAGE_VERSION 1

typedef uint8_t StoredGColor;

typedef struct PACKED StoredVector {
    float x;
    union {
        float y, z;
    };
} StoredVector;

typedef struct PACKED StoredTexCoord {
    StoredVector start;
    StoredVector end;
} StoredTexCoord;

typedef struct PACKED StoredLocation {
    int sector;
    StoredVector position;
    float height;
    float angle;
} StoredLocation;

typedef struct StoredWall {
    StoredVector startCorner;
    int portalTo;
    TextureId texture;
    StoredTexCoord texCoord;
} StoredWall;

typedef struct PACKED StoredSector {
    int wallOffset;
    int wallCount;
    int height;
    int heightOffset;
    StoredGColor floorColor;
    StoredGColor ceilColor;
} StoredSector;

typedef struct PACKED StoredLevel {
    int storageVersion;
    int sectorCount;
    int totalWallCount;
    StoredLocation playerStart;
} StoredLevel;

#endif
