#define _CRT_NONSTDC_NO_DEPRECATE
#include "levelmanager.h"
#include <stdlib.h>
#include <assert.h>
#include <parson.h>

#define LEVEL_MANAGER_CHUNK 32

typedef struct LoadedLevel
{
    Level level;
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
    }
    free(level->sectors);
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

bool levelManager_parseGColor(GColor* color, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONArray)
        return false;
    JSON_Array* array = json_value_get_array(value);
    if (json_array_get_count(array) < 3 ||
        json_array_get_count(array) > 4 ||
        json_value_get_type(json_array_get_value(array, 0)) != JSONNumber ||
        json_value_get_type(json_array_get_value(array, 1)) != JSONNumber ||
        json_value_get_type(json_array_get_value(array, 2)) != JSONNumber)
        return false;

    color->r = clampi(0, (int)json_array_get_number(array, 0), 3);
    color->g = clampi(0, (int)json_array_get_number(array, 1), 3);
    color->b = clampi(0, (int)json_array_get_number(array, 2), 3);

    if (json_array_get_count(array) == 4) {
        if (json_value_get_type(json_array_get_value(array, 3)) != JSONNumber)
            return false;
        color->a = clampi(0, (int)json_array_get_number(array, 3), 3);
    }

    return true;
}

bool levelManager_parseXZ(xz_t* xz, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONArray)
        return false;
    JSON_Array* array = json_value_get_array(value);
    if (json_array_get_count(array) != 2 ||
        json_value_get_type(json_array_get_value(array, 0)) != JSONNumber ||
        json_value_get_type(json_array_get_value(array, 1)) != JSONNumber)
        return false;

    xz->x = real_from_float((float)json_array_get_number(array, 0));
    xz->z = real_from_float((float)json_array_get_number(array, 1));
    return true;
}

bool levelManager_parseXY(xy_t* xy, JSON_Value* value)
{
    xz_t xz;
    if (!levelManager_parseXZ(&xz, value))
        return false;
    xy->x = xz.x;
    xy->y = xz.z;
    return true;
}

bool levelManager_parseTexCoord(TexCoord* texCoord, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONObject)
        return false;
    JSON_Object* object = json_value_get_object(value);
    return
        levelManager_parseXY(&texCoord->start, json_object_get_value(object, "start")) &&
        levelManager_parseXY(&texCoord->end, json_object_get_value(object, "end"));
}

bool levelManager_parseLocation(Location* location, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONObject)
        return false;
    JSON_Object* object = json_value_get_object(value);
    if (!json_object_has_value_of_type(object, "height", JSONNumber) ||
        !json_object_has_value_of_type(object, "angle", JSONNumber) ||
        !levelManager_parseXZ(&location->position, json_object_get_value(object, "position")))
        return false;

    location->height = real_from_float((float)json_object_get_number(object, "height"));
    location->angle = real_degToRad(real_from_float((float)json_object_get_number(object, "angle")));
    location->sector = -1;

    if (json_object_has_value(object, "sector")) {
        if (!json_object_has_value_of_type(object, "sector", JSONNumber))
            return false;
        location->sector = max(-1, (int)json_object_get_number(object, "sector"));
    }

    return true;
}

bool levelManager_parseWall(Wall* wall, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONObject)
        return false;
    JSON_Object* object = json_value_get_object(value);
    if (!json_object_has_value_of_type(object, "startCorner", JSONNumber) ||
        !levelManager_parseTexCoord(&wall->texCoord, json_object_get_value(object, "texCoord")))
        return false;

    wall->startCorner = (int)json_object_get_number(object, "startCorner");
    wall->texture = -1;
    wall->color = (GColor) { .argb = 255 };

    if (json_object_has_value_of_type(object, "texture", JSONNumber))
        wall->texture = (TextureId)json_object_get_number(object, "texture");

    if (json_object_has_value(object, "color") &&
        !levelManager_parseGColor(&wall->color, json_object_get_value(object, "color")))
        return false;

    if (json_object_has_value(object, "portalTo")) {
        if (!json_object_has_value_of_type(object, "portalTo", JSONNumber))
            return false;
        wall->portalTo = max(-1, (int)json_object_get_number(object, "portalTo"));
    }

    return true;
}

bool levelManager_parseSector(Sector* sector, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONObject)
        return false;
    JSON_Object* object = json_value_get_object(value);
    if (!json_object_has_value_of_type(object, "height", JSONNumber) ||
        !json_object_has_value_of_type(object, "heightOffset", JSONNumber) ||
        !json_object_has_value_of_type(object, "walls", JSONArray) ||
        !levelManager_parseGColor(&sector->floorColor, json_object_get_value(object, "floorColor")) ||
        !levelManager_parseGColor(&sector->ceilColor, json_object_get_value(object, "ceilColor")))
        return false;

    sector->height = (int)json_object_get_number(object, "height");
    sector->heightOffset = (int)json_object_get_number(object, "heightOffset");
    JSON_Array* jsonWalls = json_object_get_array(object, "walls");
    size_t wallCount = json_array_get_count(jsonWalls);
    sector->walls = (Wall*)malloc(sizeof(Wall) * wallCount);
    sector->wallCount = (int)wallCount;
    if (sector->walls == NULL)
        return false;

    for (size_t i = 0; i < json_array_get_count(jsonWalls); i++)
    {
        if (!levelManager_parseWall(&sector->walls[i], json_array_get_value(jsonWalls, i)))
            return false;
    }

    return true;
}

bool levelManager_parseLevel(Level* level, JSON_Value* value)
{
    if (value == NULL || json_value_get_type(value) != JSONObject)
        return false;
    JSON_Object* object = json_value_get_object(value);
    if (!json_object_has_value_of_type(object, "vertices", JSONArray) ||
        !json_object_has_value_of_type(object, "sectors", JSONArray) ||
        !levelManager_parseLocation(&level->playerStart, json_object_get_value(object, "playerStart")))
        return false;

    JSON_Array* jsonVertices = json_object_get_array(object, "vertices");
    JSON_Array* jsonSectors = json_object_get_array(object, "sectors");
    size_t vertexCount = json_array_get_count(jsonVertices);
    size_t sectorCount = json_array_get_count(jsonSectors);
    level->vertices = (xz_t*)malloc(sizeof(xz_t) * vertexCount);
    level->vertexCount = vertexCount;
    level->sectors = (Sector*)malloc(sizeof(Sector) * sectorCount);
    level->sectorCount = (int)sectorCount;
    if (level->sectors == NULL || level->vertices == NULL)
        return false;

    for (size_t i = 0; i < vertexCount; i++)
    {
        if (!levelManager_parseXZ(level->vertices + i, json_array_get_value(jsonVertices, i)))
            return false;
    }

    for (size_t i = 0; i < sectorCount; i++)
    {
        if (!levelManager_parseSector(&level->sectors[i], json_array_get_value(jsonSectors, i)))
            return false;
    }

    if (level->playerStart.sector < 0)
        level->playerStart.sector = level_findSectorAt(level, level->playerStart.position);

    return true;
}

LevelId levelManager_registerFile(LevelManager* me, const char* filename)
{
    assert(me != NULL && filename != NULL);
    char filenameBuffer[512];
    snprintf(filenameBuffer, 512, "%s%s", me->baseDirectory, filename);
    JSON_Value* root = json_parse_file_with_comments(filenameBuffer);
    if (root == NULL)
        return INVALID_LEVEL_ID;

    if (me->count == me->capacity)
    {
        me->capacity += LEVEL_MANAGER_CHUNK;
        me->levels = (LoadedLevel*)realloc(me->levels, sizeof(LoadedLevel) * me->capacity);
        assert(me->levels != NULL);
    }
    LevelId levelId = me->count;
    LoadedLevel* loadedLevel = &me->levels[levelId];
    loadedLevel->source = strdup(filename);
    if (!levelManager_parseLevel(&loadedLevel->level, root))
        return INVALID_LEVEL_ID;

    me->count++;
    return levelId;
}
