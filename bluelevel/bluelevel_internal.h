#ifndef BLUELEVEL_INTERNAL_H
#define BLUELEVEL_INTERNAL_H
#include "algebra.h"
#include "texture.h"
#include "bluelist.h"

typedef enum BluePointStatus {
    BluePointStatus_Ok,
    BluePointStatus_NonConvex
} BluePointStatus;

typedef struct BluePoint
{
    int refCount;
    xz_t position;

    BluePointStatus status;
} BluePoint;

typedef struct BlueSegment
{
    char* name;
    BlueList(int)* pointIds;
    real_t offset, height;
    TextureId floorTexture, ceilTexture;
    //Matrix3x2 floorTexMatrix, ceilTexMatrix;

    BlueList(BluePoint*)* pointCache;
    int pointCacheTag;
} BlueSegment;

typedef struct BlueLevel
{
    char* name, *filename;
    BlueList(BluePoint)* points;
    BlueList(BlueSegment)* segments;
} BlueLevel;

#endif
