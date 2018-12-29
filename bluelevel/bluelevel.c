#include "bluelevel_internal.h"
#include "blueutil.h"
#include "platform.h"

BlueLevel* blueLevel_new()
{
    BlueLevel* level = (BlueLevel*)blueSafeAlloc(sizeof(BlueLevel));
    level->name = blueSafeStrdup("Level");
    level->fileName = blueSafeStrdup("");
    level->points = blueList_new(BluePoint);
    level->sectors = blueList_new(BlueSector*);
    blueList_setDestructor(BlueSector, level->sectors, blueSector_deinit, NULL);

    // Create default level
    BluePoint* points[4] = {
        blueLevel_addPointAt(level, xz(real_from_int(-20), real_from_int(-20))),
        blueLevel_addPointAt(level, xz(real_from_int( 20), real_from_int(-20))),
        blueLevel_addPointAt(level, xz(real_from_int( 20), real_from_int(20))),
        blueLevel_addPointAt(level, xz(real_from_int(-20), real_from_int(20)))
    };
    BlueSector* sector = blueLevel_addSectorFromTriangle(level, points[0], points[1], points[2]);
    blueSector_addSegmentTo(sector, points[3]);
    return level;
}

void blueLevel_free(BlueLevel* level)
{
    assert(level != NULL);
    blueSafeFree(level->name);
    blueSafeFree(level->fileName);
    blueList_free(level->points);
    blueList_free(level->sectors);
    blueSafeFree(level);
}

void blueLevel_setName(BlueLevel* level, const char* newName)
{
    assert(level != NULL && newName != NULL);
    level->name = blueSafeStrdup(newName);
}

BluePoint* blueLevel_addPointAt(BlueLevel* level, xz_t position)
{
    assert(level != NULL);
    const BluePoint point = bluePoint_new(level, position);
    int index = blueList_add(level->points, &point);
    return blueList_get(BluePoint, level->points, index);
}

BlueSector* blueLevel_addSectorFromTriangle(BlueLevel* level,
    const BluePoint* p1, const BluePoint* p2, const BluePoint* p3)
{
    assert(level != NULL && p1 != NULL && p2 != NULL && p3 != NULL);
    const int index = blueList_addEmpty(level->sectors);
    BlueSector* const sector = blueList_get(BlueSector, level->sectors, index);
    blueSector_init(sector, level);
    return sector;
}

void blueLevel_removePoint(BlueLevel* level, const BluePoint* point)
{
    assert(level != NULL && point != NULL);
    assert(point->refCount > 0);
    blueList_removeByPtr(level->points, point);
}

void blueLevel_removeSector(BlueLevel* level, const BlueSector* sector)
{
    assert(level != NULL && sector != NULL);
    assert(sector->refCount > 0);
    blueList_removeByPtr(level->sectors, sector);
}

const BlueList(BluePoint)* blueLevel_getPoints(const BlueLevel* level)
{
    assert(level != NULL);
    return level->points;
}

const BlueList(BlueSector)* blueLevel_getSectors(const BlueLevel* level)
{
    assert(level != NULL);
    return level->sectors;
}

const char* blueLevel_getFileName(const BlueLevel* level)
{
    assert(level != NULL);
    return level->fileName;
}
