#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "stb.include.h"
#include "platform.h"
#include "texgen/texgen.h"

#define OPTSREADER_IMPLEMENTATION
#include "optsReader.h"

/*static const char* help_text =
    "usage: texgencli {<option> [<argument>]}\n"
    "options:\n"
    "  -h --help         Shows this help text and stops\n"
    "  -l --list         Prints list all available generators\n"
    "  -i --info <gid>   Prints all info about a generator\n"
    "  -g <gid>          Uses specified generator\n"
    "  --generator\n"
    "  -s --size <int>   Sets size (has to be power of two)\n"
    "  -pi <pid> <int>   Sets integer parameter\n"
    "  --param-int\n"
    "  -pf <pid> <float> Sets floating-point parameter\n"
    "  --param-float\n"
    "  -pb <pid> <bool>  Sets boolean parameter\n"
    "  --param-bool\n"
    "  -bi <path>        Loads parameter block from file\n"
    "  --block-input\n"
    "  -bo <path>        Saves parameter block to file\n"
    "  --block-output\n"
    "  -gi <path>        Loads generation-specifying file\n"
    "  --gen-input\n"
    "  -go <path>        Saves generation-specifying file\n"
    "  --gen-output\n"
    "  -o <path>         Saves generated texture to file (as PNG)\n"
    "  --output\n"
    "\n";*/

static const TexGeneratorID InvalidGeneratorID = TexGeneratorID(0, 0, 0, 0);
//static const TexGenParamID InvalidGenParamID = TexGenParamID(0, 0, 0, 0);

typedef struct TexGenCLI
{
    TexGeneratorInfo generator;
    TexGenerationContext* generationContext;
    int size;
} TexGenCLI;

bool texgencli_is_pot(int number)
{
    int pot = 1;
    while (pot != 0 && pot != number)
        pot <<= 1;
    return pot == number;
}

void texgencli_print_fourcc(const char* codes)
{
    int len = strlen(codes);
    putchar('{');
    for (int i = 0; i < 4; i++)
        putchar(len > i ? codes[i] : ' ');
    putchar('}');
}

void texgencli_print_generator_header(const TexGeneratorInfo* info)
{
    texgencli_print_fourcc(info->id.fourcc);
    printf(" \"%s\" - %s\n", info->name, info->description);
}

void texgencli_print_generator_param(const TexGeneratorParameterInfo* info)
{
    static const char* TYPE_NAMES[] = {
        [TexGenParamType_Int] =   "int  ",
        [TexGenParamType_Float] = "float",
        [TexGenParamType_Bool] =  "bool "
    };
    printf("  - %s ", TYPE_NAMES[info->type]);
    texgencli_print_fourcc(info->id.fourcc);
    printf(" \"%s\" - %s\n", info->name, info->description);
}

bool texgencli_parse_int(const char* param, int* value)
{
    char* end;
    *value = (int)strtol(param, &end, 0);
    bool success = param != end && errno != ERANGE;
    if (!success)
        printf("Could not parse integer: %s\n", param);
    return success;
}

bool texgencli_parse_float(const char* param, float* value)
{
    char* end;
    *value = strtof(param, &end);
    bool success = param != end && errno != ERANGE;
    if (!success)
        printf("Could not parse floating-point: %s\n", param);
    return success;
}

bool texgencli_parse_bool(const char* param, bool* value)
{
    *value =
        strcmp(param, "1") == 0 ||
        strcmp(param, "true") == 0 ||
        strcmp(param, "TRUE") == 0 ||
        strcmp(param, "yes") == 0 ||
        strcmp(param, "YES") == 0;
    return true;
}

bool texgencli_parse_fourcc(const char* param, char* fourcc)
{
    int len = strlen(param);
    if (len > 4)
        return false;
    for (int i = 0; i < 4; i++)
        fourcc[i] = len > i ? param[i] : '\0';
    return true;
}

bool texgencli_find_generator(TexGeneratorInfo* info, const char* param)
{
    TexGeneratorID id;
    if (texgencli_parse_fourcc(param, id.fourcc) &&
        texgen_getGeneratorByID(info, id))
        return true;

    int count = texgen_getGeneratorCount();
    for (int i = 0; i < count; i++)
    {
        if (texgen_getGeneratorByIndex(info, i) &&
            strcmp(info->name, param) == 0)
            return true;
    }

    printf("Could not find generator: %s\n", param);
    return false;
}

bool texgencli_require_context(TexGenCLI* cli)
{
    if (cli->generationContext != NULL)
        return true;
    if (cli->generator.id.raw == InvalidGeneratorID.raw)
    {
        puts("no generator specified");
        return false;
    }
    if (cli->size < 0)
    {
        puts("no texture size specified");
        return false;
    }

    cli->generationContext = texgen_init(NULL, cli->generator.id, cli->size);
    return cli->generationContext != NULL;
}

bool texgencli_opt_list_generators(const char* const * params, void* userdata)
{
    UNUSED(params, userdata);
    int count = texgen_getGeneratorCount();
    for (int i = 0; i < count; i++)
    {
        TexGeneratorInfo generator;
        if (texgen_getGeneratorByIndex(&generator, i))
            texgencli_print_generator_header(&generator);
    }
    return true;
}

bool texgencli_opt_generator_info(const char* const * params, void* userdata)
{
    UNUSED(userdata);
    TexGeneratorInfo info;
    if (!texgencli_find_generator(&info, params[0]))
        return false;

    texgencli_print_generator_header(&info);
    for (int i = 0; i < info.paramCount; i++)
    {
        TexGeneratorParameterInfo paramInfo;
        if (texgen_getParameterByIndex(&paramInfo, info.id, i))
            texgencli_print_generator_param(&paramInfo);
    }
    return true;
}

bool texgencli_opt_use_generator(const char* const * params, void* userdata)
{
    TexGeneratorInfo info;
    if (!texgencli_find_generator(&info, params[0]))
        return false;

    TexGenCLI* cli = (TexGenCLI*)userdata;
    if (cli->generationContext != NULL)
    {
        texgen_free(cli->generationContext);
        cli->generationContext = NULL;
    }

    cli->generator = info;
    return true;
}

bool texgencli_opt_set_size(const char* const * params, void* userdata)
{
    TexGenCLI* cli = (TexGenCLI*)userdata;
    if (!texgencli_parse_int(params[0], &cli->size))
        return false;
    if (!texgencli_is_pot(cli->size))
    {
        puts("Invalid size");
        return false;
    }
    return true;
}

static const OptionsSpecification texgencli_spec = {
    .extraHelpText =
        "<gid> / <pid> - may be name or FourCC (if printable)\n"
        "<int> - may be signed or unsigned (range -2^31 -> 2^31-1)\n"
        "<bool> - everything other than '1', 'true' or 'yes' is false\n"
        "<path> - may be stdin/stdout",
    .handlers = {
        {
            .opt = "-h|--help",
            .description = "- Shows this help text and stops",
            .callback = OptionHelpCallback
        },
        {
            .opt = "-l|--list",
            .description = "- Prints list of all available generators",
            .callback = texgencli_opt_list_generators
        },
        {
            .opt = "-i|--info",
            .description = "<gid> - Prints all info about a generator",
            .callback = texgencli_opt_generator_info,
            .paramCount = 1
        },
        {
            .opt = "-g|--generator",
            .description = "<gid> - Uses specified generator further on",
            .callback = texgencli_opt_use_generator,
            .paramCount = 1
        },
        {
            .opt = "-s|--size",
            .description = "<int> - Sets size (has to be power of two)",
            .callback = texgencli_opt_set_size,
            .paramCount = 1
        },
        { .isLast = true }
    }
};

int main(int argc, char* argv[])
{
    TexGenCLI cli = {
        .generator = { .id = InvalidGeneratorID },
        .generationContext = NULL,
        .size = -1
    };
    memset(&cli, 0, sizeof(cli));

    readOptions(argc, argv, &texgencli_spec, &cli);

    if (cli.generationContext != NULL)
    {
        texgen_free(cli.generationContext);
        cli.generationContext = NULL;
    }
    return 0;
}

const Texture* texture_load(TextureManagerHandle manager, TextureId id)
{
    UNUSED(manager, id);
    return NULL;
}

const Texture* texture_createEmpty(TextureManagerHandle manager, GSize size, GColor** contentPtr)
{
    UNUSED(manager);
    static TextureId nextId = 0;
    Texture* texture = (Texture*)malloc(sizeof(Texture));
    texture->pixels = (GColor*)malloc(sizeof(GColor) * size.w * size.h);
    texture->id = nextId++;
    texture->size = size;
    assert(texture != NULL && texture->pixels != NULL);

    if (contentPtr != NULL)
        *contentPtr = texture->pixels;
    return texture;
}

void texture_free(TextureManagerHandle manager, const Texture* texture)
{
    UNUSED(manager);
    if (texture == NULL)
        return;
    if (texture->pixels != NULL)
        free(texture->pixels);
    free((void*)texture);
}
