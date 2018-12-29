#include "bluelevel_internal.h"
#include <assert.h>

BluePoint bluePoint_new(BlueLevel* level, xz_t pos) {
    assert(level != NULL);
    return (BluePoint) {
        .refCount = 0,
        .position = pos,
        .level = level ,
        .status = BluePointStatus_Unused
    };
}

xz_t* bluePoint_modifyPosition(BluePoint* point) {
    assert(point != NULL);
    return &point->position;
}

BluePointStatus bluePoint_getStatus(const BluePoint* point) {
    assert(point != NULL);
    return point->status;
}
