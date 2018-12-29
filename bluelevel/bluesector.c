#include "bluelevel_internal.h"
#include "blueutil.h"
#include "platform.h"
#include <assert.h>

#define BLUE_SECTOR_DEFAULT_HEIGHT real_from_int(20)

BlueSlabTexMatrix BlueSlabTexMatrix_identity() {
    const BlueSlabTexMatrix identity = {
        .offset = xy_zero,
        .scale = xy_one,
        .rotation = real_zero
    };
    return identity;
}

void blueSector_init(BlueSector* sector, BlueLevel* level)
{
    assert(sector != NULL && level != NULL);
    sector->name = blueSafeStrdup("Sector");
    sector->level = level;
    sector->height = BLUE_SECTOR_DEFAULT_HEIGHT;
    sector->floorTexture = INVALID_TEXTURE_ID;
    sector->ceilTexture = INVALID_TEXTURE_ID;
    sector->floorTexMatrix = BlueSlabTexMatrix_identity();
    sector->ceilTexMatrix = BlueSlabTexMatrix_identity();
    sector->segments = blueList_new(BlueSegment);
}

void blueSector_deinit(BlueSector* sector, void* _)
{
    UNUSED(_);
    assert(sector != NULL);
    blueSafeFree(sector->name);
    blueList_free(sector->segments);
}

void blueSector_setName(BlueSector* sector, const char* newName)
{
    assert(sector != NULL && newName != NULL);
    blueSafeFree(sector->name);
    sector->name = blueSafeStrdup(newName);
}

real_t* blueSector_modifyOffset(BlueSector* sector)
{
    assert(sector != NULL);
    return &sector->offset;
}

real_t* blueSector_modifyHeight(BlueSector* sector)
{
    assert(sector != NULL);
    return &sector->height;
}

void blueSector_setFloorTexture(BlueSector* sector, TextureId newTexture)
{
    assert(sector != NULL && newTexture != INVALID_TEXTURE_ID);
    sector->floorTexture = newTexture;
    sector->floorTexMatrix = BlueSlabTexMatrix_identity();
}

void blueSector_setCeilTexture(BlueSector* sector, TextureId newTexture)
{
    assert(sector != NULL && newTexture != INVALID_TEXTURE_ID);
    sector->ceilTexture = newTexture;
    sector->ceilTexMatrix = BlueSlabTexMatrix_identity();
}

BlueSlabTexMatrix* blueSector_modifyFloorTexMatrix(BlueSector* sector)
{
    assert(sector != NULL);
    return &sector->floorTexMatrix;
}

BlueSlabTexMatrix* blueSector_modifyCeilTexMatrix(BlueSector* sector)
{
    assert(sector != NULL);
    return &sector->ceilTexMatrix;
}

BlueSegment* blueSector_addSegmentTo(BlueSector* sector, const BluePoint* point)
{
    assert(sector != NULL && point != NULL);
    const BlueEntryID pointId = blueList_getIdByPtr(sector->level->points, point);
    const BlueSegment segment = blueSegment_new(sector, pointId);
    int index = blueList_add(sector->segments, &segment);
    return blueList_get(BlueSegment, sector->segments, index);
}

void blueSector_removeSegment(BlueSector* sector, const BlueSegment* seg)
{
    assert(sector != NULL && seg != NULL);
    blueList_removeByPtr(sector->segments, seg);
}

const char* blueSector_getName(const BlueSector* sector)
{
    assert(sector != NULL);
    return sector->name;
}

TextureId blueSector_getFloorTexture(const BlueSector* sector)
{
    assert(sector != NULL);
    return sector->floorTexture;
}

TextureId blueSector_getCeilTexture(const BlueSector* sector)
{
    assert(sector != NULL);
    return sector->ceilTexture;
}

const BlueList(BlueSegment)* blueSector_getSegments(const BlueSector* sector)
{
    assert(sector != NULL);
    return sector->segments;
}
