#include "texgen_internal.h"
#include "texgen_registry.h"

void* rawtexgen_getParamPointer(const TexGeneratorParam* param, void* paramBlock)
{
    return (char*)paramBlock + param->offset;
}

int texgen_getGeneratorCount()
{
    return sizeof(rawtexgen_registry) / sizeof(TexGeneratorInitializer);
}

bool_t texgen_getGeneratorInfoByIndex(TexGeneratorInfo* info, int index)
{
    if (index < 0 || index >= texgen_getGeneratorCount())
        return false;
    const TexGenerator* generator = rawtexgen_registry[index]();
    memcpy(info, &generator->info, sizeof(TexGeneratorInfo));
    return true;
}

const TexGenerator* rawtexgen_getGeneratorByID(TexGeneratorID id)
{
    const int count = texgen_getGeneratorCount();
    for (int i = 0; i < count; i++)
    {
        const TexGenerator* generator = rawtexgen_registry[i]();
        if (generator->info.id == id)
            return generator;
    }
    return NULL;
}

bool_t texgen_getGeneratorInfoByID(TexGeneratorInfo* info, TexGeneratorID id)
{
    const TexGenerator* generator = rawtexgen_getGeneratorByID(id);
    if (generator != NULL)
        memcpy(info, &generator->info, sizeof(TexGeneratorInfo));
    return (generator != NULL);
}

bool_t texgen_getParameterInfoByIndex(TexGeneratorParameterInfo* info, TexGeneratorID id, int index)
{
    const TexGenerator* generator = rawtexgen_getGeneratorByID(id);
    if (generator == NULL)
        return false;
    if (index < 0 || index >= generator->info.paramCount)
        return false;
    memcpy(info, &generator->params[index], sizeof(TexGeneratorParameterInfo));
    return true;
}

const TexGeneratorParam* rawtexgen_getParameterByID(const TexGenerator* generator, TexGenParamID paramId)
{
    const TexGeneratorParam* curParam = generator->params;
    for (int i = 0; i < generator->info.paramCount; i++, curParam++)
    {
        if (curParam->info.id == paramId)
            return curParam;
    }
    return NULL;
}

bool_t texgen_getParameterInfoByID(TexGeneratorParameterInfo* info, TexGeneratorID genId, TexGenParamID paramId)
{
    const TexGenerator* generator = rawtexgen_getGeneratorByID(genId);
    if (generator == NULL)
        return false;
    const TexGeneratorParam* param = rawtexgen_getParameterByID(generator, paramId);
    if (param != NULL)
        memcpy(info, &param->info, sizeof(TexGeneratorParameterInfo));
    return (param != NULL);
}

static int prv_abs_to_logsize(int absSize)
{
    int logSize = 1;
    while ((1 << logSize) != 0 && (1 << logSize) != absSize)
        logSize++;
    return logSize;
}

static bool_t prv_texgen_resetToGenerator(TexGenerationContext* me, const TexGenerator* generator)
{
    if (me->generator == generator)
        return true;
    if (me->paramBlock != NULL)
        free(me->paramBlock);
    me->paramBlock = malloc(generator->info.paramBlockSize);
    if (me->paramBlock == NULL)
    {
        texgen_free(me);
        return false;
    }
    memcpy(me->paramBlock, generator->defaultParamBlock, generator->info.paramBlockSize);

    me->generator = generator;
    return true;
}

TexGenerationContext* texgen_init(TextureManagerHandle textureManager, TexGeneratorID id, int absSize)
{
    int logSize = prv_abs_to_logsize(absSize);
    if (logSize == 0)
        return NULL;

    const TexGenerator* generator = rawtexgen_getGeneratorByID(id);
    if (generator == NULL)
        return NULL;

    TexGenerationContext* me = (TexGenerationContext*)malloc(sizeof(TexGenerationContext));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(TexGenerationContext));

    me->texture = texture_createEmpty(textureManager, (GSize) { absSize, absSize }, &me->pixels);
    if (me->texture == NULL)
    {
        texgen_free(me);
        return NULL;
    }

    if (!prv_texgen_resetToGenerator(me, generator))
    {
        texgen_free(me);
        return NULL;
    }
    me->textureManager = textureManager;
    me->logSize = logSize;
    return me;
}

void texgen_free(TexGenerationContext* me)
{
    if (me == NULL)
        return;
    if (me->texture != NULL)
        texture_free(me->textureManager, me->texture);
    if (me->paramBlock != NULL)
        free(me->paramBlock);
    free(me);
}

void texgen_setSize(TexGenerationContext* ctx, int newAbsSize)
{
    ctx->logSize = prv_abs_to_logsize(newAbsSize);
    assert(ctx->logSize != 0);
    texture_resizeEmpty(ctx->textureManager, ctx->texture->id, GSize(newAbsSize, newAbsSize), &ctx->pixels);
}

void texgen_setGenerator(TexGenerationContext* ctx, TexGeneratorID id)
{
    const TexGenerator* generator = rawtexgen_getGeneratorByID(id);
    if (generator != NULL)
        prv_texgen_resetToGenerator(ctx, generator);
}

void texgen_setParamInt(TexGenerationContext* me, TexGenParamID id, int value)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Int)
    {
        int* targetPtr = (int*)rawtexgen_getParamPointer(param, me->paramBlock);
        *targetPtr = value;
    }
}

void texgen_setParamFloat(TexGenerationContext* me, TexGenParamID id, float value)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Float)
    {
        float* targetPtr = (float*)rawtexgen_getParamPointer(param, me->paramBlock);
        *targetPtr = value;
    }
}

void texgen_setParamBool(TexGenerationContext* me, TexGenParamID id, bool_t value)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Bool)
    {
        bool_t* targetPtr = (bool_t*)rawtexgen_getParamPointer(param, me->paramBlock);
        *targetPtr = value;
    }
}

void texgen_setParamColor(TexGenerationContext* me, TexGenParamID id, GColor value)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Color)
    {
        GColor* targetPtr = (GColor*)rawtexgen_getParamPointer(param, me->paramBlock);
        *targetPtr = value;
    }
}

void texgen_setParams(TexGenerationContext* me, const void* paramBlock)
{
    memcpy(me->paramBlock, paramBlock, me->generator->info.paramBlockSize);
}

int texgen_getLogSize(TexGenerationContext* ctx)
{
    return ctx->logSize;
}

TexGeneratorID texgen_getGenerator(TexGenerationContext* ctx)
{
    return ctx->generator->info.id;
}

int texgen_getParamInt(TexGenerationContext* me, TexGenParamID id)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Int)
    {
        int* targetPtr = (int*)rawtexgen_getParamPointer(param, me->paramBlock);
        return *targetPtr;
    }
    return 0;
}

float texgen_getParamFloat(TexGenerationContext* me, TexGenParamID id)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Float)
    {
        float* targetPtr = (float*)rawtexgen_getParamPointer(param, me->paramBlock);
        return *targetPtr;
    }
    return 0;
}

bool_t texgen_getParamBool(TexGenerationContext* me, TexGenParamID id)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Bool)
    {
        bool_t* targetPtr = (bool_t*)rawtexgen_getParamPointer(param, me->paramBlock);
        return *targetPtr;
    }
    return false;
}

GColor texgen_getParamColor(TexGenerationContext* me, TexGenParamID id)
{
    const TexGeneratorParam* param = rawtexgen_getParameterByID(me->generator, id);
    if (param != NULL && param->info.type == TexGenParamType_Color)
    {
        GColor* targetPtr = (GColor*)rawtexgen_getParamPointer(param, me->paramBlock);
        return *targetPtr;
    }
    return GColorFromRGBA(0, 0, 0, 0);
}

void texgen_getParams(TexGenerationContext* me, void* outParamBlock)
{
    memcpy(outParamBlock, me->paramBlock, me->generator->info.paramBlockSize);
}

bool_t texgen_execute(TexGenerationContext* me)
{
    return me->generator->callback(me->pixels, me->logSize, me->paramBlock);
}

TextureId texgen_getTextureId(TexGenerationContext* me)
{
    return me->texture->id;
}

const Texture* texgen_getTexture(TexGenerationContext* me)
{
    return me->texture;
}
