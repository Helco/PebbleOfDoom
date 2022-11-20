#define _CRT_NONSTDC_NO_DEPRECATE
#include "levelmanager.h"
#include <stdlib.h>
#include <assert.h>
#include <parson.h>

#define LEVEL_MANAGER_CHUNK 32

typedef struct LoadedLevel
{
    Level level;
    LevelId id;
    char* source;
} LoadedLevel;

struct LevelManager
{
    char* baseDirectory;
    int count, capacity;
    LoadedLevel* levels;
};

LevelManager* levelManager_init(const char* baseDirectory)
{
    LevelManager* me = (LevelManager*)malloc(sizeof(LevelManager));
    if (me == NULL)
        return NULL;
    me->levels = (LoadedLevel*)malloc(sizeof(LoadedLevel) * LEVEL_MANAGER_CHUNK);
    if (me->levels == NULL) {
        free(me);
        return NULL;
    }

    me->baseDirectory = strdup(baseDirectory);
    me->count = 0;
    me->capacity = LEVEL_MANAGER_CHUNK;
    return me;
}

void levelManager_freeLevel(Level* level)
{
    for (int sectorI = 0; sectorI < level->sectorCount; sectorI++)
    {
        Sector* sector = &level->sectors[sectorI];
        free(sector->walls);
        free(sector->entities);
    }
    free(level->sectors);
    free(level->vertices);
}

void levelManager_free(LevelManager* me)
{
    assert(me != NULL);
    for (int i = 0; i < me->count; i++)
    {
        levelManager_freeLevel(&me->levels[i].level);
        free(me->levels[i].source);
    }
    free(me->levels);
    free(me->baseDirectory);
    free(me);
}

int levelManager_getLevelCount(LevelManager* me)
{
    assert(me != NULL);
    return me->count;
}

const Level* levelManager_getLevelByIndex(LevelManager* me, int index)
{
    assert(me != NULL);
    assert(index >= 0 && index < me->count);
    return &me->levels[index].level;
}

const Level* levelManager_getLevelById(LevelManager* me, LevelId id)
{
    for (int i = 0; i < me->count; i++)
    {
        if (me->levels[i].id == id)
            return &me->levels[i].level;
    }
    assert(false && "Invalid level id");
    return NULL;
}

#include "levelstorage.h"

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

bool prv_convert_vertex(xz_t* vertex, FILE* fp)
{
    StoredVector storedVector;
    if (fread(&storedVector, sizeof(StoredVector), 1, fp) != 1) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level vertex");
        return false;
    }

    *vertex = prv_convert_xz(storedVector);
    return true;
}

bool prv_convert_sector(Sector* sector, FILE* fp)
{
    StoredSector storedSector;
    if (fread(&storedSector, sizeof(StoredSector), 1, fp) != 1) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level Sector");
        return false;
    }

    sector->walls = (Wall*)malloc(sizeof(Wall) * storedSector.wallCount);
    if (sector->walls == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate sector walls");
        return false;
    }

    sector->entities = (Entity*)malloc(sizeof(Entity) * storedSector.entityCount);
    if (sector->entities == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate sector entities");
        return false;
    }

    sector->wallCount = storedSector.wallCount;
    sector->entityCount = storedSector.entityCount;
    sector->height = storedSector.height;
    sector->heightOffset = storedSector.heightOffset;
    sector->floorColor = prv_convert_color(storedSector.floorColor);
    sector->ceilColor = prv_convert_color(storedSector.ceilColor);
    return true;
}

bool prv_convert_wall(Wall* wall, FILE* fp)
{
    StoredWallV4 storedWall;
    if (fread(&storedWall, sizeof(StoredWallV4), 1, fp) != 1) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level wall");
        return false;
    }

    wall->portalTo = storedWall.portalTo;
    wall->startCorner = storedWall.startCorner;
    wall->texture = storedWall.texture;
    wall->texCoord = prv_convert_texCoord(storedWall.texCoord);
    wall->color = prv_convert_color(storedWall.color);
    wall->flags = storedWall.flags;
    return true;
}

bool prv_convert_entity(Entity* entity, FILE* fp)
{
    StoredEntity storedEntity;
    if (fread(&storedEntity, sizeof(StoredEntity), 1, fp) != 1) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level entity");
        return false;
    }

    entity->location = prv_convert_location(storedEntity.location);
    entity->sprite = storedEntity.sprite;
    entity->type = storedEntity.type;
    entity->arg1 = storedEntity.arg1;
    entity->arg2 = storedEntity.arg2;
    entity->arg3 = storedEntity.arg3;
    return true;
}

bool levelManager_loadLevel(Level* level, FILE* fp)
{
    StoredLevel storedLevel;
    if (fread(&storedLevel, sizeof(StoredLevel), 1, fp) != 1) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read level header");
        return false;
    }

    if (storedLevel.storageVersion != LEVEL_STORAGE_VERSION_V4) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown level storage version %d (should be %d)", storedLevel.storageVersion, LEVEL_STORAGE_VERSION_V4);
        return NULL;
    }

    Sector* sectors = level->sectors = (Sector*)malloc(sizeof(Sector) * storedLevel.sectorCount);
    xz_t* vertices = level->vertices = (xz_t*)malloc(sizeof(xz_t) * storedLevel.vertexCount);
    if (sectors == NULL || vertices == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate %d sectors, %d walls and %d vertices", storedLevel.sectorCount, storedLevel.totalWallCount, storedLevel.vertexCount);
        return NULL;
    }
    level->sectorCount = storedLevel.sectorCount;
    level->vertexCount = storedLevel.vertexCount;
    level->playerStart = prv_convert_location(storedLevel.playerStart);

    for (int i = 0; i < storedLevel.vertexCount; i++)
        if (!prv_convert_vertex(vertices + i, fp))
            return false;

    for (int i = 0; i < storedLevel.sectorCount; i++)
        if (!prv_convert_sector(sectors + i, fp))
            return false;

    for (int i = 0; i < storedLevel.sectorCount; i++)
    {
        Sector* sector = level->sectors + i;
        for (int j = 0; j < sector->wallCount; j++)
            if (!prv_convert_wall(sector->walls + j, fp))
                return false;
    }

    for (int i = 0; i < storedLevel.sectorCount; i++)
    {
        Sector* sector = level->sectors + i;
        for (int j = 0; j < sector->entityCount; j++)
            if (!prv_convert_entity(sector->entities + j, fp))
                return false;
    }

    return true;
}

LevelId levelManager_registerFile(LevelManager* me, const char* filename, LevelId id)
{
    assert(me != NULL && filename != NULL);
    char filenameBuffer[512];
    snprintf(filenameBuffer, 512, "%s%s", me->baseDirectory, filename);
    FILE* file;
#ifdef _MSC_VER
    file = NULL;
    fopen_s(&file, filenameBuffer, "rb");
#else
    file = fopen(filename, "rb");
#endif
    if (file == NULL)
        return INVALID_LEVEL_ID;

    if (me->count == me->capacity)
    {
        me->capacity += LEVEL_MANAGER_CHUNK;
        me->levels = (LoadedLevel*)realloc(me->levels, sizeof(LoadedLevel) * me->capacity);
        assert(me->levels != NULL);
    }
    int levelIndex = me->count;
    LoadedLevel* loadedLevel = &me->levels[levelIndex];
    loadedLevel->id = id;
    loadedLevel->source = strdup(filename);
    if (!levelManager_loadLevel(&loadedLevel->level, file))
        return INVALID_LEVEL_ID;
    fclose(file);

    me->count++;
    return id;
}
