#ifndef TEXGEN_H
#define TEXGEN_H
#include "../platform.h"
#include "../algebra.h"
#include "../texture.h"

/* This is a small library to define texture generators
 * using macro meta-programming.
 * Note that only square power-of-two textures are supported!
 * See texgen_xor for an example
 */

typedef uint32_t TexGeneratorID;
#define TexGeneratorID(a,b,c,d) FOURCC(a,b,c,d)

typedef uint32_t TexGenParamID;
#define TexGenParamID(a,b,c,d) FOURCC(a,b,c,d)

typedef enum TexGeneratorParamType {
    TexGenParamType_Int = 0,
    TexGenParamType_Float,
    TexGenParamType_Bool,
    TexGenParamType_Color
} TexGeneratorParamType;

typedef struct TexGeneratorInfo {
    TexGeneratorID id;
    const char* name;
    const char* description;
    int paramCount;
    int paramBlockSize;
} TexGeneratorInfo;

typedef struct TexGeneratorParameterInfo {
    TexGenParamID id;
    const char* name;
    const char* description;
    TexGeneratorParamType type;
    struct {
        union {
            int integer;
            float floating;
        } min, max;
    } range;
} TexGeneratorParameterInfo;

typedef struct TexGenerationContext TexGenerationContext;

int texgen_getGeneratorCount();
bool_t texgen_getGeneratorInfoByIndex(TexGeneratorInfo* info, int index);
bool_t texgen_getGeneratorInfoByID(TexGeneratorInfo* info, TexGeneratorID id);
bool_t texgen_getParameterInfoByIndex(TexGeneratorParameterInfo* info, TexGeneratorID id, int index);
bool_t texgen_getParameterInfoByID(TexGeneratorParameterInfo* info, TexGeneratorID genId, TexGenParamID paramId);

TexGenerationContext* texgen_init(TextureManagerHandle textureManager, TexGeneratorID id, int size);
void texgen_free(TexGenerationContext* ctx);
void texgen_setSize(TexGenerationContext* ctx, int newSize);
void texgen_setGenerator(TexGenerationContext* ctx, TexGeneratorID id);
void texgen_setParamInt(TexGenerationContext* ctx, TexGenParamID id, int value);
void texgen_setParamFloat(TexGenerationContext* ctx, TexGenParamID id, float value);
void texgen_setParamBool(TexGenerationContext* ctx, TexGenParamID id, bool_t value);
void texgen_setParamColor(TexGenerationContext* ctx, TexGenParamID id, GColor value);
void texgen_setParams(TexGenerationContext* ctx, const void* paramBlock);
int texgen_getLogSize(TexGenerationContext* ctx);
TexGeneratorID texgen_getGenerator(TexGenerationContext* ctx);
int texgen_getParamInt(TexGenerationContext* ctx, TexGenParamID id);
float texgen_getParamFloat(TexGenerationContext* ctx, TexGenParamID id);
bool_t texgen_getParamBool(TexGenerationContext* ctx, TexGenParamID id);
GColor texgen_getParamColor(TexGenerationContext* ctx, TexGenParamID id);
void texgen_getParams(TexGenerationContext* ctx, void* outParamBlock);
bool_t texgen_execute(TexGenerationContext* ctx);
TextureId texgen_getTextureId(TexGenerationContext* ctx);
const Texture* texgen_getTexture(TexGenerationContext* ctx);

#define TexGenerator_XOR (TexGeneratorID('X', 'O', 'R', '\0'))
#define TexGenParam_XOR_UseColor (TexGenParamID('C', 'O', 'L', '\0'))

#define TexGenerator_Rand (TexGeneratorID('R', 'A', 'N', 'D'))
#define TexGenParam_Rand_UseColor (TexGenParamID('U', 'C', 'O', 'L'))
#define TexGenParam_Rand_UseSeed (TexGenParamID('U', 'S', 'E', 'E'))
#define TexGenParam_Rand_Seed (TexGenParamID('S', 'E', 'E', 'D'))

#define TexGenerator_Bricks (TexGeneratorID('B', 'R', 'I', 'K'))
#define TexGenParam_Bricks_BrickWidth (TexGenParamID('B', 'R', 'W', '\0'))
#define TexGenParam_Bricks_BrickHeight (TexGenParamID('B', 'R', 'H', '\0'))
#define TexGenParam_Bricks_BrickColor (TexGenParamID('B', 'R', 'C', '\0'))
#define TexGenParam_Bricks_BrickNoise (TexGenParamID('B', 'R', 'N', '\0'))
#define TexGenParam_Bricks_BorderSize (TexGenParamID('B', 'O', 'R', 'S'))
#define TexGenParam_Bricks_BorderWalls (TexGenParamID('B', 'O', 'R', 'W'))
#define TexGenParam_Bricks_BorderColor (TexGenParamID('B', 'O', 'R', 'C'))
#define TexGenParam_Bricks_BorderNoise (TexGenParamID('B', 'O', 'R', 'N'))

#define TexGenerator_SolidColor (TexGeneratorID('S', 'C', 'O', 'L'))
#define TexGenParam_SolidColor_Color (TexGenParamID('C', 'O', 'L', '\0'))

#endif
