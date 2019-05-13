#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "platform.h"
#include "levelmanager.h"
#include "levelstorage.h"

#define OPTSREADER_IMPLEMENTATION
#include "optsReader.h"

bool checkPlatformTraits()
{
    // checks byte-endian, integer size and floating-point representation
    union {
        unsigned int integer;
        float floating;
    } v;
    v.floating = 42.0f;
    return
        sizeof(v) == 4 &&
        v.integer == 0x42280000;
}

StoredVector pbllevelconvert_convertXZ(xz_t xz)
{
    return (StoredVector) {
        .x = real_to_float(xz.x),
        .z = real_to_float(xz.z)
    };
}

StoredVector pbllevelconvert_convertXY(xy_t xy)
{
    return (StoredVector) {
        .x = real_to_float(xy.x),
        .y = real_to_float(xy.y)
    };
}

StoredGColor pbllevelconvert_convertColor(GColor color)
{
    return color.argb;
}

StoredTexCoord pbllevelconvert_convertTexCoord(TexCoord texCoord)
{
    return (StoredTexCoord) {
        .start = pbllevelconvert_convertXY(texCoord.start),
        .end = pbllevelconvert_convertXY(texCoord.end)
    };
}

StoredLocation pbllevelconvert_convertLocation(Location loc)
{
    return (StoredLocation) {
        .sector = loc.sector,
        .height = real_to_float(loc.height),
        .angle = real_to_float(loc.angle),
        .position = pbllevelconvert_convertXZ(loc.position)
    };
}

void pbllevelconvert_writeWall(const Wall* wall, FILE* output)
{
    StoredWall storedWall = {
        .startCorner = wall->startCorner,
        .portalTo = wall->portalTo,
        .texture = wall->texture,
        .texCoord = pbllevelconvert_convertTexCoord(wall->texCoord)
    };
    fwrite(&storedWall, sizeof(StoredWall), 1, output);
}

void pbllevelconvert_writeSector(const Sector* sector, int wallOffset, FILE* output)
{
    StoredSector storedSector = {
        .wallOffset = wallOffset,
        .wallCount = sector->wallCount,
        .height = sector->height,
        .heightOffset = sector->heightOffset,
        .floorColor = pbllevelconvert_convertColor(sector->floorColor),
        .ceilColor = pbllevelconvert_convertColor(sector->ceilColor)
    };
    fwrite(&storedSector, sizeof(StoredSector), 1, output);
}

typedef struct PebbleLevelConvert {
    const char* inputFilename;
    const char* outputFilename;
} PebbleLevelConvert;

bool pbllevelconvert_opt_input(const char* const * params, void* userdata)
{
    PebbleLevelConvert* context = (PebbleLevelConvert*)userdata;
    context->inputFilename = params[0];
    return true;
}

bool pbllevelconvert_opt_output(const char* const * params, void* userdata)
{
    PebbleLevelConvert* context = (PebbleLevelConvert*)userdata;
    context->outputFilename = params[0];
    return true;
}

static const OptionsSpecification pbllevelconvert_spec = {
    .extraHelpText = "",
    .handlers = {
        {
            .opt = "-h|--help",
            .description = " - Shows this help text and stops",
            .callback = OptionHelpCallback
        },
        {
            .opt = "-i|--input",
            .description = " - Sets the input level file",
            .callback = pbllevelconvert_opt_input,
            .paramCount = 1
        },
        {
            .opt = "-o|--output",
            .description = " - Sets the output level file",
            .callback = pbllevelconvert_opt_output,
            .paramCount = 1
        },
        { .isLast = true }
    }
};

void pbllevelconvert_writeLevel(const Level* level, FILE* output)
{
    StoredLevel storedLevel = {
        .storageVersion = LEVEL_STORAGE_VERSION,
        .sectorCount = level->sectorCount,
        .vertexCount = level->vertexCount,
        .playerStart = pbllevelconvert_convertLocation(level->playerStart),
        .totalWallCount = 0
    };
    for (int i = 0; i < level->sectorCount; i++)
        storedLevel.totalWallCount += level->sectors[i].wallCount;

    fwrite(&storedLevel, sizeof(StoredLevel), 1, output);
    for (int i = 0; i < level->vertexCount; i++) {
        StoredVector vertex = pbllevelconvert_convertXZ(level->vertices[i]);
        fwrite(&vertex, sizeof(StoredVector), 1, output);
    }
    int wallOffset = 0;
    for (int i = 0; i < level->sectorCount; i++) {
        pbllevelconvert_writeSector(&level->sectors[i], wallOffset, output);
        wallOffset += level->sectors[i].wallCount;
    }
    for (int i = 0; i < level->sectorCount; i++) {
        for (int j = 0; j < level->sectors[i].wallCount; j++)
            pbllevelconvert_writeWall(&level->sectors[i].walls[j], output);
    }
}

int main(int argc, char** argv)
{
    PebbleLevelConvert context;
    memset(&context, 0, sizeof(PebbleLevelConvert));
    readOptions(argc, argv, &pbllevelconvert_spec, &context);

    if (!checkPlatformTraits()) {
        fputs("This tool only supports pebble-like platforms (32Bit compatible, little endian).", stderr);
        return -1;
    }

    if (context.inputFilename == NULL) {
        fputs("No input file given.", stderr);
        return -1;
    }

    LevelManager* levelManager = levelManager_init("");
    if (levelManager == NULL) {
        fputs("Could not initiate level manager.", stderr);
        return -1;
    }

    LevelId levelId = levelManager_registerFile(levelManager, context.inputFilename);
    if (levelId == INVALID_LEVEL_ID) {
        fputs("Could not load level.", stderr);
        return -1;
    }
    const Level* level = levelManager_getLevelByIndex(levelManager, levelId);

    bool shouldCloseOutput = false;
    FILE* output = stdout;
    if (context.outputFilename != NULL) {
        shouldCloseOutput = true;
        output = fopen(context.outputFilename, "wb");
        if (!output) {
            fputs("Could not open output file.", stderr);
            return -1;
        }
    }

    pbllevelconvert_writeLevel(level, output);

    if (shouldCloseOutput)
        fclose(output);
    levelManager_free(levelManager);
    return 0;
}
