#ifndef BLUEGEOMETRY_H
#define BLUEGEOMETRY_H
#include "bluelevel.h"

lineSeg_t blueSegment_asLineSegment(const BlueSegment* seg);
xz_t blueSegment_getDirection(const BlueSegment* seg);
bool blueSegment_isPointInside(const BlueSegment* seg, xz_t point);
real_t blueSegment_getAngleToPrev(const BlueSegment* seg);

void blueSector_convexHullSort(BlueSector* sector);
bool blueSector_containsPoint(const BlueSector* sector, xz_t point);
bool blueSector_isConvex(const BlueSector* sector);

#endif
