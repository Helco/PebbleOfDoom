#ifndef BLUELEVEL_H
#define BLUELEVEL_H
#include "algebra.h"
#include "texture.h"
#include "bluelist.h"

typedef enum BluePointStatus {
    BluePointStatus_Ok,
    BluePointStatus_NonConvex,
    BluePointStatus_Unused
} BluePointStatus;

typedef enum BlueSegmentStatus {
    BlueSegmentStatus_Ok,
    BlueSegmentStatus_NonConvex,
    BlueSegmentStatus_MissingTexture
} BlueSegmentStatus;

typedef enum BlueSectorStatus {
    BlueSectorStatus_Ok,
    BlueSectorStatus_NoArea,
    BlueSectorStatus_MissingTexture
} BlueSectorStatus;

typedef struct BlueSlabTexMatrix {
    xy_t offset;
    xy_t scale;
    real_t rotation;
} BlueSlabTexMatrix;
BlueSlabTexMatrix BlueSlabTexMatrix_identity();

typedef struct BlueTexCoords {
    xy_t start;
    xy_t end;
} BlueTexCoords;

typedef struct BluePoint BluePoint;
xz_t* bluePoint_modifyPosition(BluePoint* point);
BluePointStatus bluePoint_getStatus(const BluePoint* point);

typedef struct BlueSegment BlueSegment;
BlueTexCoords* blueSegment_modifyTexCoords(BlueSegment* seg);
void blueSegment_setTexture(BlueSegment* seg, TextureId newTexture);
const BluePoint* blueSegment_getStartPoint(const BlueSegment* seg);
TextureId blueSegment_getTexture(const BlueSegment* seg);
BlueSegmentStatus blueSegment_getStatus(const BlueSegment* seg);

typedef struct BlueSector BlueSector;
void blueSector_setName(BlueSector* sector, const char* newName);
real_t* blueSector_modifyOffset(BlueSector* sector);
real_t* blueSector_modifyHeight(BlueSector* sector);
void blueSector_setFloorTexture(BlueSector* sector, TextureId newTexture);
void blueSector_setCeilTexture(BlueSector* sector, TextureId newTexture);
BlueSlabTexMatrix* blueSector_modifyFloorTexMatrix(BlueSector* sector);
BlueSlabTexMatrix* blueSector_modifyCeilTexMatrix(BlueSector* sector);
BlueSegment* blueSector_addSegmentTo(BlueSector* sector, const BluePoint* point);
void blueSector_removeSegment(BlueSector* sector, const BlueSegment* seg);
const char* blueSector_getName(const BlueSector* sector);
TextureId blueSector_getFloorTexture(const BlueSector* sector);
TextureId blueSector_getCeilTexture(const BlueSector* sector);
const BlueList(BlueSegment)* blueSector_getSegments(const BlueSector* sector);

typedef struct BlueLevel BlueLevel;
BlueLevel* blueLevel_new();
void blueLevel_free(BlueLevel* level);
void blueLevel_setName(BlueLevel* level, const char* newName);
BluePoint* blueLevel_addPointAt(BlueLevel* level, xz_t position);
BlueSector* blueLevel_addSectorFromTriangle(BlueLevel* level,
    const BluePoint* p1,const BluePoint* p2, const BluePoint* p3);
void blueLevel_removePoint(BlueLevel* level, const BluePoint* point);
void blueLevel_removeSector(BlueLevel* level, const BlueSector* sector);
const BlueList(BluePoint)* blueLevel_getPoints(const BlueLevel* level);
const BlueList(BlueSector)* blueLevel_getSectors(const BlueLevel* level);
const char* blueLevel_getFileName(const BlueLevel* level);

#endif
