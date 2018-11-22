#include "texgen_internal.h"

int texgen_getGeneratorCount();
bool_t texgen_getGeneratorByIndex(TexGeneratorInfo* info, int index);
bool_t texgen_getGeneratorByID(TexGeneratorInfo* info, TexGeneratorID id);
bool_t texgen_getParameterByIndex(TexGeneratorParameterInfo* info, TexGeneratorID id, int index);
bool_t texgen_getParameterByID(TexGeneratorParameterInfo* info, TexGeneratorID genId, TexGenParamID paramId);

TexGenerationContext* texgen_init(TexGeneratorID id, int size);
void texgen_free(TexGenerationContext* ctx);
void texgen_setParamInt(TexGenerationContext* ctx, TexGenParamID id, int value);
void texgen_setParamReal(TexGenerationContext* ctx, TexGenParamID id, real_t value);
void texgen_setParamBool(TexGenerationContext* ctx, TexGenParamID id, bool_t value);
void texgen_setParams(TexGenerationContext* ctx, const void* paramBlock);
void texgen_getParams(TexGenerationContext* ctx, void* outParamBlock);
void texgen_execute(TexGenerationContext* ctx);
const Texture* texgen_getTexture(TexGenerationContext* ctx);
