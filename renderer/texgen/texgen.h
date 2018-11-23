#ifndef TEXGEN_H
#define TEXGEN_H
#include "../algebra.h"
#include "../texture.h"

/* This is a small library to define texture generators
 * using macro meta-programming.
 * Note that only square power-of-two textures are supported!
 * See texgen_xor for an example
 */

typedef union TexGeneratorID {
    uint32_t raw;
    char fourcc[4];
} TexGeneratorID;

#define TexGeneratorID(a,b,c,d) \
    (((TexGeneratorID){ .fourcc = { (a), (b), (c), (d) } }))

typedef union TexGenParamID {
    uint32_t raw;
    char fourcc[4];
} TexGenParamID;

#define TexGenParamID(a,b,c,d) \
    (((TexGenParamID){ .fourcc = { (a), (b), (c), (d) } }))

typedef enum TexGeneratorParamType {
    TexGenParamType_Int = 0,
    TexGenParamType_Float,
    TexGenParamType_Bool,
} TexGeneratorParamType;

typedef struct TexGeneratorInfo {
    TexGeneratorID id;
    const char* description;
    int paramCount;
    int paramBlockSize;
} TexGeneratorInfo;

typedef struct TexGeneratorParameterInfo {
    TexGenParamID id;
    const char* name;
    const char* description;
    TexGeneratorParamType type;
} TexGeneratorParameterInfo;

typedef struct TexGenerationContext TexGenerationContext;

int texgen_getGeneratorCount();
bool_t texgen_getGeneratorByIndex(TexGeneratorInfo* info, int index);
bool_t texgen_getGeneratorByID(TexGeneratorInfo* info, TexGeneratorID id);
bool_t texgen_getParameterByIndex(TexGeneratorParameterInfo* info, TexGeneratorID id, int index);
bool_t texgen_getParameterByID(TexGeneratorParameterInfo* info, TexGeneratorID genId, TexGenParamID paramId);

TexGenerationContext* texgen_init(TextureManagerHandle textureManager, TexGeneratorID id, int size);
void texgen_free(TexGenerationContext* ctx);
void texgen_setParamInt(TexGenerationContext* ctx, TexGenParamID id, int value);
void texgen_setParamFloat(TexGenerationContext* ctx, TexGenParamID id, float value);
void texgen_setParamBool(TexGenerationContext* ctx, TexGenParamID id, bool_t value);
void texgen_setParams(TexGenerationContext* ctx, const void* paramBlock);
void texgen_getParams(TexGenerationContext* ctx, void* outParamBlock);
bool_t texgen_execute(TexGenerationContext* ctx);
TextureId texgen_getTextureId(TexGenerationContext* ctx);

#define TexGenerator_XOR (TexGeneratorID('X', 'O', 'R', '\0'))
#define TexGenParam_XOR_UseColor (TexGenParamID('C', 'O', 'L', '\0'))

#define TexGenerator_Rand (TexGeneratorID('R', 'A', 'N', 'D'))
#define TexGenParam_Rand_UseColor (TexGenParamID('U', 'C', 'O', 'L'))
#define TexGenParam_Rand_UseSeed (TexGenParamID('U', 'S', 'E', 'E'))
#define TexGenParam_Rand_Seed (TexGenParamID('S', 'E', 'E', 'D'))

#endif
