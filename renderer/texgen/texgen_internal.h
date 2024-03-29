#ifndef TEXGEN_INTERNAL_H
#define TEXGEN_INTERNAL_H
#include "texgen.h"

#define TEXGENERATOR_LOGTOABSSIZE(logSize) ((GSize) { (1 << (logSize)), (1 << (logSize)) })
#define TEXGENERATOR_OFFSETOF(type,member) ((int)(intptr_t)(&((type*)0)->member))

#define BEGIN_TEXGENERATOR(name,type) \
    typedef type _texgen_type; \
    TEXGENERATOR_HEADER(name) { \
        static const TexGeneratorParam _texgeneratorParams_##name [] = {
#define TEXGENPARAM(_id,_description,_typeenum,_member,_min,_max) { \
    .info = { \
        .id = _id, \
        .name = #_member, \
        .description = _description, \
        .type = _typeenum, \
        .range = { .min = _min, .max = _max } \
    }, \
    .offset = TEXGENERATOR_OFFSETOF(_texgen_type, _member) \
    },
#define TEXGENPARAM_INT(id,description,member,_min,_max) \
    TEXGENPARAM(id, description, TexGenParamType_Int, member, \
        { .integer = _min }, \
        { .integer = _max })
#define TEXGENPARAM_FLOAT(id,description,member,_min,_max) \
    TEXGENPARAM(id, description, TexGenParamType_Float, member, \
        { .floating = _min }, \
        { .floating = _max })
#define TEXGENPARAM_BOOL(id,description,member) \
    TEXGENPARAM(id, description, TexGenParamType_Bool, member, { .integer = 0 }, { .integer = 1 })
#define TEXGENPARAM_COLOR(id,description,member) \
    TEXGENPARAM(id, description, TexGenParamType_Color, member, { .integer = 0 }, { .integer = 255 })

#define END_TEXGENERATOR(_name,_id,_description,_defaultParams,_callback) \
    }; \
    static TexGenerator generator = { .callback = NULL }; \
    if (generator.callback == NULL) { \
        generator.info.id = _id; \
        generator.info.name = #_name; \
        generator.info.description = _description; \
        generator.info.paramCount = sizeof(_texgeneratorParams_##_name) / sizeof(TexGeneratorParam); \
        generator.info.paramBlockSize = sizeof(_texgen_type); \
        generator.callback = _callback; \
        generator.params = _texgeneratorParams_##_name; \
        generator.defaultParamBlock = &_defaultParams; \
    } \
    return &generator; \
    }

#define TEXGENERATOR_SYMBOL(_name) _texgenerator_##_name
#define TEXGENERATOR_HEADER(_name) \
    const TexGenerator* TEXGENERATOR_SYMBOL(_name) ()

typedef bool_t (*TexGeneratorCallback)(GColor* output, int logSize, const void* params);

typedef struct TexGeneratorParam {
    TexGeneratorParameterInfo info;
    int offset;
} TexGeneratorParam;

typedef struct TexGenerator {
    TexGeneratorInfo info;
    TexGeneratorCallback callback;
    const TexGeneratorParam* params;
    const void* defaultParamBlock;
} TexGenerator;

struct TexGenerationContext {
    TextureManagerHandle textureManager;
    const TexGenerator* generator;
    const Texture* texture;
    GColor* pixels;
    void* paramBlock;
    int logSize;
};

typedef const TexGenerator* (*TexGeneratorInitializer)();

#endif
