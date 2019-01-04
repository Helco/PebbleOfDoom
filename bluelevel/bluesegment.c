#include "bluelevel_internal.h"
#include <assert.h>

BlueSegment blueSegment_new(BlueSector* sector, BlueEntryID startPointId)
{
    return (BlueSegment) {
        .startPointId = startPointId,
        .texture = INVALID_TEXTURE_ID,
        .texCoords = (BlueTexCoords) {
            .start = xy_zero,
            .end = xy_one
        },
        .portalTargetId = INVALID_BLUEENTRYID,
        .sector = sector,
        .status = BlueSegmentStatus_MissingTexture
    };
}

BlueTexCoords* blueSegment_modifyTexCoords(BlueSegment* seg)
{
    assert(seg != NULL);
    return &seg->texCoords;
}

void blueSegment_setTexture(BlueSegment* seg, TextureId newTexture)
{
    assert(seg != NULL);
    assert(newTexture != INVALID_TEXTURE_ID);
    seg->texture = newTexture;
    seg->texCoords = (BlueTexCoords) {
        .start = xy_zero,
        .end = xy_one
    };
}

const BluePoint* blueSegment_getStartPoint(const BlueSegment* seg)
{
    assert(seg != NULL);
    int index = blueList_findById(seg->sector->level->points, seg->startPointId);
    return index < 0 ? NULL : blueList_get(BluePoint, seg->sector->level->points, index);
}

TextureId blueSegment_getTexture(const BlueSegment* seg)
{
    assert(seg != NULL);
    return seg->texture;
}

BlueSegmentStatus blueSegment_getStatus(const BlueSegment* seg)
{
    assert(seg != NULL);
    return seg->status;
}

const BlueSegment* blueSegment_getNext(const BlueSegment* seg)
{
    assert(seg != NULL);
    int index = blueList_findByPtr(seg->sector->segments, seg);
    index = (index + 1) % blueList_getCount(seg->sector->segments);
    return blueList_get(BlueSegment, seg->sector->segments, index);
}

const BlueSegment* blueSegment_getPrev(const BlueSegment* seg)
{
    assert(seg != NULL);
    int index = blueList_findByPtr(seg->sector->segments, seg);
    index = index > 0 ? index - 1 : blueList_getCount(seg->sector->segments) - 1;
    return blueList_get(BlueSegment, seg->sector->segments, index);
}
