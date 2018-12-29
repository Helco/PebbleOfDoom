#ifndef BLUELEVEL_INTERNAL_H
#define BLUELEVEL_INTERNAL_H
#include "bluelevel.h"
#include "bluelist.h"

struct BluePoint
{
    xz_t position;

    BlueLevel* level;
    BluePointStatus status;
    int refCount;
};

struct BlueSegment
{
    BlueEntryID startPointId;
    TextureId texture;
    BlueTexCoords texCoords;
    BlueEntryID portalTargetId;

    BlueSector* sector;
    BlueSegmentStatus status;
};

struct BlueSector
{
    char* name;
    BlueList(BlueSegment)* segments;
    real_t offset, height;
    TextureId floorTexture, ceilTexture;
    BlueSlabTexMatrix floorTexMatrix, ceilTexMatrix;

    BlueLevel* level;
    BlueSectorStatus status;
    int refCount;
};

struct BlueLevel
{
    char* name, *fileName;
    BlueList(BluePoint)* points;
    BlueList(BlueSector)* sectors;
};

BluePoint bluePoint_new(BlueLevel* level, xz_t pos);

BlueSegment blueSegment_new(BlueSector* sector, BlueEntryID startPointId);

void blueSector_init(BlueSector* sector, BlueLevel* level);
void blueSector_deinit(BlueSector* sector, void* _);

#endif
