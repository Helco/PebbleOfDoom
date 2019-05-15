#include "levelresources.h"
#include "levelstorage.h"
#include "../renderer/platform.h"

static LevelId loadedLevelId = INVALID_LEVEL_ID;
static Level loadedLevel;
static Sector* sectors = NULL;
static Wall* walls = NULL;
static xz_t* vertices = NULL;

GColor prv_convert_color(StoredGColor g)
{
    return (GColor) { .argb = g };
}

xz_t prv_convert_xz(StoredVector v)
{
    return xz(real_from_float(v.x), real_from_float(v.z));
}

xy_t prv_convert_xy(StoredVector v)
{
    return xy(real_from_float(v.x), real_from_float(v.y));
}

TexCoord prv_convert_texCoord(StoredTexCoord c)
{
    return (TexCoord) {
        .start = prv_convert_xy(c.start),
        .end = prv_convert_xy(c.end)
    };
}

Location prv_convert_location(StoredLocation location)
{
    return (Location) {
        .position = prv_convert_xz(location.position),
        .sector = location.sector,
        .angle = real_from_float(location.angle),
        .height = real_from_float(location.height)
    };
}

size_t prv_convert_vertex(xz_t* vertex, ResHandle handle, size_t offset)
{
    StoredVector storedVector;
    if (resource_load_byte_range(handle, offset, (uint8_t*)&storedVector, sizeof(StoredVector)) != sizeof(StoredVector)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level vertex");
        return offset + sizeof(StoredVector);
    }

    *vertex = prv_convert_xz(storedVector);
    return offset + sizeof(StoredVector);
}

size_t prv_convert_wall(Wall* wall, ResHandle handle, size_t offset)
{
    StoredWall storedWall;
    if (resource_load_byte_range(handle, offset, (uint8_t*)&storedWall, sizeof(StoredWall)) != sizeof(StoredWall)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level wall");
        return offset + sizeof(StoredWall);
    }

    wall->portalTo = storedWall.portalTo;
    wall->startCorner = storedWall.startCorner;
    wall->texture = storedWall.texture;
    wall->texCoord = prv_convert_texCoord(storedWall.texCoord);
    wall->color = prv_convert_color(storedWall.color);
    return offset + sizeof(StoredWall);
}

size_t prv_convert_sector(Sector* sector, ResHandle handle, size_t offset)
{
    StoredSector storedSector;
    if (resource_load_byte_range(handle, offset, (uint8_t*)&storedSector, sizeof(StoredSector)) != sizeof(StoredSector)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level Sector");
        return offset + sizeof(StoredSector);
    }

    sector->walls = walls + storedSector.wallOffset;
    sector->wallCount = storedSector.wallCount;
    sector->height = storedSector.height;
    sector->heightOffset = storedSector.heightOffset;
    sector->floorColor = prv_convert_color(storedSector.floorColor);
    sector->ceilColor = prv_convert_color(storedSector.ceilColor);
    return offset + sizeof(StoredSector);
}

LevelId loadLevelFromResource(uint32_t resourceId)
{
    ResHandle handle = resource_get_handle(resourceId);
    return handle == NULL ? INVALID_LEVEL_ID : (LevelId)resourceId;
}

const Level* level_load(LevelManagerHandle lvlManager, LevelId id)
{
    level_free(NULL, &loadedLevel);

    ResHandle handle = resource_get_handle((uint32_t)id);
    if (handle == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not get resource handle for level %d", id);
        return NULL;
    }

    size_t size = resource_size(handle);
    if (size < sizeof(StoredLevel)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Resource is too small (%d bytes) for level %d", size, id);
        return NULL;
    }

    size_t offset = 0;
    StoredLevel storedLevel;
    if (resource_load(handle, (uint8_t*)&storedLevel, sizeof(StoredLevel)) != sizeof(StoredLevel)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level header for %d", id);
        return NULL;
    }
    offset += sizeof(storedLevel);

    if (storedLevel.storageVersion != LEVEL_STORAGE_VERSION) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown level storage version %d (should be %d)", storedLevel.storageVersion, LEVEL_STORAGE_VERSION);
        return NULL;
    }

    sectors = (Sector*)malloc(sizeof(Sector) * storedLevel.sectorCount);
    walls = (Wall*)malloc(sizeof(Wall) * storedLevel.totalWallCount);
    vertices = (xz_t*)malloc(sizeof(xz_t) * storedLevel.vertexCount);
    if (sectors == NULL || walls == NULL || vertices == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate %d sectors, %d walls and %d vertices", storedLevel.sectorCount, storedLevel.totalWallCount, storedLevel.vertexCount);
        return NULL;
    }

    loadedLevelId = id;
    loadedLevel.sectorCount = storedLevel.sectorCount;
    loadedLevel.sectors = sectors;
    loadedLevel.playerStart = prv_convert_location(storedLevel.playerStart);
    loadedLevel.vertexCount = storedLevel.vertexCount;
    loadedLevel.vertices = vertices;

    for (int i = 0; i < storedLevel.vertexCount; i++)
        offset = prv_convert_vertex(vertices + i, handle, offset);

    for (int i = 0; i < storedLevel.sectorCount; i++)
        offset = prv_convert_sector(sectors + i, handle, offset);

    for (int i = 0; i < storedLevel.totalWallCount; i++)
        offset = prv_convert_wall(walls + i, handle, offset);

    return &loadedLevel;
}

void level_free(LevelManagerHandle lvlManger, const Level* level)
{
    if (loadedLevelId == INVALID_LEVEL_ID || level != &loadedLevel)
        return;
    free(sectors);
    sectors = NULL;
    free(walls);
    walls = NULL;
    memset(&loadedLevel, 0, sizeof(Level));
    loadedLevelId = 0;
}
