#include "bluegeometry.h"
#include "bluelevel_internal.h"
#include "platform.h"
#include <assert.h>
#include <math.h>

lineSeg_t blueSegment_asLineSegment(const BlueSegment* seg)
{
    assert(seg != NULL);
    lineSeg_t lineSeg;
    lineSeg.start.xz = blueSegment_getStartPoint(seg)->position;
    lineSeg.end.xz = blueSegment_getStartPoint(blueSegment_getNext(seg))->position;
    return lineSeg;
}

xz_t blueSegment_getDirection(const BlueSegment* seg)
{
    assert(seg != NULL);
    lineSeg_t lineSeg = blueSegment_asLineSegment(seg);
    return xz_sub(lineSeg.end.xz, lineSeg.start.xz);
}

bool blueSegment_isPointInside(const BlueSegment* seg, xz_t point)
{
    assert(seg != NULL);
    const lineSeg_t lineSeg = blueSegment_asLineSegment(seg);
    return xz_isOnRight(point, lineSeg);
}

real_t blueSegment_getAngleToPrev(const BlueSegment* seg)
{
    assert(seg != NULL);
    const xz_t prevDir = blueSegment_getDirection(blueSegment_getPrev(seg));
    const xz_t myDir = blueSegment_getDirection(seg);
    const real_t divisor = real_mul(xz_length(prevDir), xz_length(myDir));
    const real_t cosAngle = real_div(xz_dot(prevDir, myDir), divisor);
    return real_from_float(acosf(real_to_float(cosAngle)));
}

bool blueSector_containsPoint(const BlueSector* sector, xz_t point)
{
    assert(sector != NULL);
    blueList_foreach(BlueSegment, segment, sector->segments) {
        if (!blueSegment_isPointInside(segment, point))
            return false;
    }
    return true;
}

bool blueSector_isConvex(const BlueSector* sector)
{
    assert(sector != NULL);
    blueList_foreach(BlueSegment, segment, sector->segments) {
        const xz_t nextPoint = blueSegment_getStartPoint(blueSegment_getNext(segment))->position;
        if (!blueSegment_isPointInside(segment, nextPoint))
            return false;
    }
    return true;
}

static int blueSector_convexHullSort_pointComparer(const int* a, const int* b, void* userdata)
{
    const BlueSector* sector = (const BlueSector*)userdata;
    const BlueSegment* const aSegment = blueList_get(BlueSegment, sector->segments, *a);
    const BlueSegment* const bSegment = blueList_get(BlueSegment, sector->segments, *b);
    const xz_t aPos = blueSegment_getStartPoint(aSegment)->position;
    const xz_t bPos = blueSegment_getStartPoint(bSegment)->position;

    int xComparison = real_compare(aPos.x, bPos.x);
    if (xComparison != 0)
        return xComparison;
    return real_compare(aPos.z, bPos.z);
}

static BlueList(int)* blueSector_convexHullSort_sortPoints(const BlueSector* sector)
{
    BlueList(int)* indices = blueList_new(int);
    for (int i = 0; i < blueList_getCount(sector->segments); i++)
        blueList_add(indices, &i);
    blueList_sort(int, indices, blueSector_convexHullSort_pointComparer, (void*)sector);
    return indices;
}

static lineSeg_t blueSector_convexHullSort_getLastSeg(const BlueSector* sector, const BlueList(int)* hull)
{
    xz_t startPos = blueList_get(BluePoint, sector->level->points,
        *blueList_get(int, hull, blueList_getCount(hull) - 2))->position;
    xz_t endPos = blueList_get(BluePoint, sector->level->points,
        *blueList_get(int, hull, blueList_getCount(hull) - 1))->position;
    return (lineSeg_t) {
        .start = { .xz = startPos },
        .end = { .xz = endPos }
    };
}

static void blueSector_convexHullSort_marchChain(BlueSector* sector, BlueList(int)* hull,
    const BlueList(int)* xSorted, int start, int step)
{
    int i = start;
    int pointsAdded = 0;
    for (i += step; i >= 0 && i < blueList_getCount(xSorted); i += step) {
        const int curPointI = *blueList_get(int, xSorted, i);
        const BluePoint* curPoint = blueList_get(BluePoint, sector->level->points, curPointI);
        while (pointsAdded >= 2) {
            lineSeg_t lastSeg = blueSector_convexHullSort_getLastSeg(sector, hull);
            if (xz_isOnRight(curPoint->position, lastSeg))
                break;
            blueList_removeByIndex(hull, blueList_getCount(hull) - 1);
        }
        blueList_add(hull, &curPointI);
        pointsAdded++;
    }
}

void blueSector_convexHullSort(BlueSector* sector)
{
    assert(sector != NULL);
    BlueList(int)* hull = blueList_new(int);
    BlueList(int)* xSorted = blueSector_convexHullSort_sortPoints(sector);
    blueSector_convexHullSort_marchChain(sector, hull, xSorted, 0, 1);
    blueSector_convexHullSort_marchChain(sector, hull, xSorted, blueList_getCount(xSorted)-1, -1);
}
