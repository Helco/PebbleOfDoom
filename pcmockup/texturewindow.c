#include "pcmockup.h"

struct TextureWindow {
    Window* window;
    TextureManager* manager;
    UploadedTexture* uploadedTexture;
    int curTextureIndex, uploadedTextureIndex;
};

void textureWindow_updateContent(Window* window, void* userdata);
void textureWindow_printImportedTexture(TextureWindow* me, const Texture* texture);
void textureWindow_printGeneratedTexture(TextureWindow* me, const Texture* texture, TexGenerationContext* generationContext);

TextureWindow* textureWindow_init(WindowContainer* parent, TextureManager* manager)
{
    TextureWindow* me = (TextureWindow*)malloc(sizeof(TextureWindow));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(TextureWindow));

    me->window = windowContainer_newWindow(parent, "Textures");
    if (me->window == NULL) {
        textureWindow_free(me);
        return NULL;
    }
    window_setFlags(me->window,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoCollapse);
    window_setOpenState(me->window, WindowOpenState_Closed);
    window_setUpdateCallbacks(me->window, (WindowUpdateCallbacks) {
        .userdata = me,
        .content = textureWindow_updateContent
    });

    me->uploadedTexture = uploadedTexture_init();
    if (me->uploadedTexture == NULL) {
        textureWindow_free(me);
        return NULL;
    }

    me->curTextureIndex = 0;
    me->manager = manager;
    me->uploadedTextureIndex = -1;
    return me;
}

void textureWindow_free(TextureWindow* me)
{
    if (me == NULL)
        return;
    if (me->uploadedTexture != NULL)
        uploadedTexture_free(me->uploadedTexture);
    free(me);
}

void textureWindow_updateTexture(TextureWindow* me)
{
    const Texture* const curTexture = textureManager_getTextureByIndex(me->manager, me->curTextureIndex);
    if (me->uploadedTextureIndex != me->curTextureIndex)
        uploadedTexture_setFromTexture(me->uploadedTexture, curTexture);
}

void textureWindow_updateContent(Window* window, void* userdata)
{
#define BUFFER_SIZE 64
    static char buffer[BUFFER_SIZE];
    UNUSED(window);
    TextureWindow* me = (TextureWindow*)userdata;
    const int textureCount = textureManager_getTextureCount(me->manager);
    const Texture* const curTexture = textureManager_getTextureByIndex(me->manager, me->curTextureIndex);
    const float contentWidth = igGetContentRegionAvailWidth();
    const float imageAspect = ((float)curTexture->size.w) / curTexture->size.h;
    const ImVec2 imageSize = { contentWidth, contentWidth / imageAspect };
    const ImVec2 zero = { 0, 0 }, one = { 1, 1 };
    const ImVec4 transparent = { 0, 0, 0, 0 }, white = { 1, 1, 1, 1 };

    if (igArrowButton("prevTex", ImGuiDir_Left) && me->curTextureIndex > 0)
        me->curTextureIndex--;
    snprintf(buffer, BUFFER_SIZE, "TextureID %%d/%d", textureCount);
    igSameLine(0.0f, -1.0f);
    igSliderInt("", &me->curTextureIndex, 0, textureCount - 1, buffer);
    igSameLine(0.0f, -1.0f);
    if (igArrowButton("nextTex", ImGuiDir_Right))
        me->curTextureIndex++;
    me->curTextureIndex = clampi(0, me->curTextureIndex, textureCount - 1);

    textureWindow_updateTexture(me);
    ImTextureID imTextureId = (ImTextureID)(intptr_t)uploadedTexture_getGLTextureId(me->uploadedTexture);
    igImageButton(imTextureId, imageSize, zero, one, 0, transparent, white);
    igSeparator();

    TexGenerationContext* const generationContext = textureManager_getGenerationContext(me->manager, curTexture->id);
    if (generationContext == NULL)
        textureWindow_printImportedTexture(me, curTexture);
    else
        textureWindow_printGeneratedTexture(me, curTexture, generationContext);
#undef BUFFER_SIZE
}

void textureWindow_printImportedTexture(TextureWindow* me, const Texture* texture)
{
    igLabelText("Source", "%s", textureManager_getTextureSource(me->manager, texture));
    igLabelText("Size", "%dx%d", texture->size.w, texture->size.h);
}

bool textureWindow_printGenerators(TexGenerationContext* generationContext)
{
    const ImVec2 defaultSize = { 0, 0 };
    const TexGeneratorID curGeneratorId = texgen_getGenerator(generationContext);
    bool shouldRegenerate = false;
    TexGeneratorInfo generatorInfo;
    texgen_getGeneratorInfoByID(&generatorInfo, curGeneratorId);
    if (!igBeginCombo("Generator", generatorInfo.name, ImGuiComboFlags_None))
        return shouldRegenerate;
    const int generatorCount = texgen_getGeneratorCount();
    for (int i = 0; i < generatorCount; i++) {
        texgen_getGeneratorInfoByIndex(&generatorInfo, i);
        const bool isCurrent = generatorInfo.id == curGeneratorId;
        if (igSelectable(generatorInfo.name, isCurrent, ImGuiSelectableFlags_None, defaultSize)) {
            texgen_setGenerator(generationContext, generatorInfo.id);
            shouldRegenerate = true;
        }
    }
    igEndCombo();
    return shouldRegenerate;
}

bool textureWindow_printGenerationSize(TexGenerationContext* generationContext)
{
#define BUFFER_SIZE 32
    bool shouldRegenerate = false;
    int logSize = texgen_getLogSize(generationContext);
    static char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%d", 1 << logSize);
    if (igSliderInt("Size", &logSize, 1, 8, buffer)) {
        texgen_setSize(generationContext, 1 << logSize);
        shouldRegenerate = true;
    }
    return shouldRegenerate;;
#undef BUFFER_SIZE
}

bool textureWindow_printIntGenerationParameter(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo)
{
    int value = texgen_getParamInt(generationContext, paramInfo->id);
    if (paramInfo->range.min.integer <= INT32_MIN / 2 || paramInfo->range.max.integer >= INT32_MAX / 2) {
        // this is a limitation of imgui :(
        if (igInputInt(paramInfo->name, &value, 0, 0, ImGuiInputTextFlags_CharsDecimal))
        {
            value = clampi(paramInfo->range.min.integer, value, paramInfo->range.max.integer);
            texgen_setParamInt(generationContext, paramInfo->id, value);
            return true;
        }
    }
    else if (igSliderInt(paramInfo->name, &value,
        paramInfo->range.min.integer, paramInfo->range.max.integer, "%d")) {
        texgen_setParamInt(generationContext, paramInfo->id, value);
        return true;
    }
    return false;
}

bool textureWindow_printFloatGenerationParameter(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo)
{
    float value = texgen_getParamFloat(generationContext, paramInfo->id);
    if (igSliderFloat(paramInfo->name, &value,
        paramInfo->range.min.floating, paramInfo->range.max.floating, "%.3f", 1.0f)) {
        texgen_setParamFloat(generationContext, paramInfo->id, value);
        return true;
    }
    return false;
}

bool textureWindow_printBoolGenerationParameter(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo)
{
    bool value = (bool)texgen_getParamBool(generationContext, paramInfo->id);
    if (igCheckbox(paramInfo->name, &value)) {
        texgen_setParamBool(generationContext, paramInfo->id, (bool_t)value);
        return true;
    }
    return false;
}

bool textureWindow_printColorGenerationParameter(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo)
{
    UNUSED(generationContext, paramInfo);
    igText("eeehm please implement");
    return false;
}

bool textureWindow_printGenerationParameters(TexGenerationContext* generationContext)
{
    typedef bool (*PrintParameterFn)(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo);
    static const PrintParameterFn printParameterFns[] = {
        [TexGenParamType_Int] =   textureWindow_printIntGenerationParameter,
        [TexGenParamType_Float] = textureWindow_printFloatGenerationParameter,
        [TexGenParamType_Bool] =  textureWindow_printBoolGenerationParameter,
        [TexGenParamType_Color] = textureWindow_printColorGenerationParameter
    };
    const TexGeneratorID generatorId = texgen_getGenerator(generationContext);
    bool shouldRegenerate = false;
    TexGeneratorInfo generatorInfo;
    texgen_getGeneratorInfoByID(&generatorInfo, generatorId);
    TexGeneratorParameterInfo paramInfo;
    for (int i = 0; i < generatorInfo.paramCount; i++) {
        texgen_getParameterInfoByIndex(&paramInfo, generatorId, i);
        shouldRegenerate = shouldRegenerate || printParameterFns[paramInfo.type](generationContext, &paramInfo);
        if (igIsItemHovered(ImGuiHoveredFlags_None))
        {
            igBeginTooltip();
            igTextUnformatted(paramInfo.description, NULL);
            igEndTooltip();
        }
    }
    return shouldRegenerate;
}

void textureWindow_printGeneratedTexture(TextureWindow* me, const Texture* texture, TexGenerationContext* generationContext)
{
    UNUSED(me, texture);
    bool shouldRegenerate = false;
    shouldRegenerate = shouldRegenerate || textureWindow_printGenerators(generationContext);
    shouldRegenerate = shouldRegenerate || textureWindow_printGenerationSize(generationContext);
    igSeparator();
    shouldRegenerate = shouldRegenerate || textureWindow_printGenerationParameters(generationContext);

    if (shouldRegenerate)
        texgen_execute(generationContext);
}

Window* textureWindow_asWindow(TextureWindow* me)
{
    return me->window;
}
