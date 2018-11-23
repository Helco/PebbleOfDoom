#ifndef TEXGEN_INTERNAL_H
#define TEXGEN_INTERNAL_H
#include "texgen.h"

#define TEXGENERATOR_LOGTOABSSIZE(logSize) ((GSize) { (1 << (logSize)), (1 << (logSize)) })
#define TEXGENERATOR_OFFSETOF(type,member) ((int)(&((type*)0)->member))

#define BEGIN_TEXGENERATOR(name) \
    static const TexGeneratorParam _texgeneratorParams_##name [] = {

#define TEXGENPARAM(_id,_description,_typeenum,_type,_member) { \
    .info = { \
        .id = _id, \
        .description = _description, \
        .type = _typeenum \
    }, \
    .offset = TEXGENERATOR_OFFSETOF(_type, _member) \
    }
#define TEXGENPARAM_INT(id,description,type,member) \
    TEXGENPARAM(id, description, TexGenParamType_Int, type, member)
#define TEXGENPARAM_FLOAT(id,description,type,member) \
    TEXGENPARAM(id, description, TexGenParamType_Float, type, member)
#define TEXGENPARAM_BOOL(id,description,type,member) \
    TEXGENPARAM(id, description, TexGenParamType_Bool, type, member)

#define END_TEXGENERATOR(_name,_id,_description,_type,_defaultParams,_callback) \
    }; \
    const TexGenerator TEXGENERATOR_SYMBOL(_name) = { \
        .info = { \
            .id = _id, \
            .description = _description, \
            .paramCount = sizeof(_texgenerator_##_name) / sizeof(TexGeneratorParam), \
            .paramBlockSize = sizeof(_type) \
        }, \
        .callback = _callback, \
        .params = _texgeneratorParams_##_name, \
        .defaultParamBlock = &_defaultParams \
    }; \

#define TEXGENERATOR_SYMBOL(_name) _texgenerator_##_name

typedef bool_t (*TexGeneratorCallback)(GColor* output, int logSize, const void* params);

typedef struct TexGeneratorParam {
    TexGeneratorParameterInfo info;
    int offset;
} TexGeneratorParam;

typedef struct TexGenerator {
    TexGeneratorInfo info;
    TexGeneratorCallback callback;
    const TexGeneratorParam* params;
} TexGenerator;

struct TexGenerationContext {
    TextureManagerHandle textureManager;
    const TexGenerator* generator;
    const Texture* texture;
    GColor* pixels;
    void* paramBlock;
    int logSize;
};

// List of registered texture generators

extern const TexGenerator TEXGENERATOR_SYMBOL(XOR);

static const TexGenerator** rawtexgen_getGenerators() {
    static const TexGenerator* generators[] = {
        &TEXGENERATOR_SYMBOL(XOR),
        NULL
    };
    return generators;
}

#endif
