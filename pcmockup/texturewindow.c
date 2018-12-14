#include "pcmockup.h"
#include <stdarg.h>
#include "stb.include.h"

struct TextureWindow {
    Window* window;
    TextureManager* manager;
    UploadedTexture* uploadedTexture;
    int curTextureIndex, uploadedTextureIndex;
    char** textureFiles; // allocated with stb_readdir_*
};

void textureWindow_free(void* userdata);
void textureWindow_updateContent(void* userdata);
void textureWindow_printImportedTexture(TextureWindow* me, const Texture* texture);
void textureWindow_printGeneratedTexture(TextureWindow* me, const Texture* texture, TexGenerationContext* generationContext);
void textureWindow_printImportFromFileMenu(TextureWindow* me);
void textureWindow_printGenerateNewMenu(TextureWindow* me);

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
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_MenuBar);
    window_setOpenState(me->window, WindowOpenState_Closed);
    window_addCallbacks(me->window, (WindowCallbacks) {
        .userdata = me,
        .destruct = textureWindow_free,
        .contentUpdate = textureWindow_updateContent
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

void textureWindow_free(void* userdata)
{
    TextureWindow* me = (TextureWindow*)userdata;
    if (me->uploadedTexture != NULL)
        uploadedTexture_free(me->uploadedTexture);
    if (me->textureFiles != NULL)
        stb_readdir_free(me->textureFiles);
    free(me);
}

void textureWindow_updateTexture(void* userdata)
{
    TextureWindow* me = (TextureWindow*)userdata;
    const Texture* const curTexture = textureManager_getTextureByIndex(me->manager, me->curTextureIndex);
    if (me->uploadedTextureIndex != me->curTextureIndex) {
        uploadedTexture_setFromTexture(me->uploadedTexture, curTexture);
        me->uploadedTextureIndex = me->curTextureIndex;
    }
}

void textureWindow_printTextureSelection(TextureWindow* me)
{
#define BUFFER_SIZE 64
    static char buffer[BUFFER_SIZE];
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
#undef BUFFER_SIZE
}

void textureWindow_updateContent(void* userdata)
{
    TextureWindow* me = (TextureWindow*)userdata;
    if (igBeginMenuBar()) {
        textureWindow_printImportFromFileMenu(me);
        textureWindow_printGenerateNewMenu(me);
        igEndMenuBar();
    }

    textureWindow_printTextureSelection(me);

    igSeparator();
    const Texture* const curTexture = textureManager_getTextureByIndex(me->manager, me->curTextureIndex);
    TexGenerationContext* const generationContext = textureManager_getGenerationContext(me->manager, curTexture->id);
    if (generationContext == NULL)
        textureWindow_printImportedTexture(me, curTexture);
    else
        textureWindow_printGeneratedTexture(me, curTexture, generationContext);
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

ImVec4 pebbleColorToImGui(GColor color)
{
    return (ImVec4) {
        color.r / 3.0f,
        color.g / 3.0f,
        color.b / 3.0f,
        color.a / 3.0f
    };
}

bool igcGColorPicker(const char* label, GColor* color)
{
    const ImGuiColorEditFlags pickerColorEditFlags =
        ImGuiColorEditFlags_NoPicker |
        ImGuiColorEditFlags_NoLabel |
        ImGuiColorEditFlags_NoTooltip;
    const ImGuiColorEditFlags btnColorEditFlags =
        ImGuiColorEditFlags_NoPicker |
        ImGuiColorEditFlags_Uint8;
    const ImVec4 imColor = pebbleColorToImGui(*color);
    bool didValueChange = false;

    igPushIDStr(label);
    if (igColorButton(label, imColor, btnColorEditFlags, (ImVec2) { 0, 0 }))
        igOpenPopup("picker");
    if (!igBeginPopup("picker", ImGuiWindowFlags_None)) {
        igPopID();
        return false;
    }
    for (int i = 0; i < 64; i++) {
        const GColor curColor = (GColor) {
            .r = (i >> 0) & 3,
            .g = (i >> 2) & 3,
            .b = (i >> 4) & 3,
            .a = 3
        };
        const ImVec4 btnColor = pebbleColorToImGui(curColor);
        igPushIDInt(i);
        if (igColorButton("", btnColor, pickerColorEditFlags, (ImVec2) { 0, 0 })) {
            didValueChange = true;
            color->r = curColor.r;
            color->g = curColor.g;
            color->b = curColor.b;
        }
        if ((i % 8) < 7)
            igSameLine(0.0f, -1.0f);
        igPopID();
    }
    int alpha = color->a;
    if (igSliderInt("", &alpha, 0, 3, "Alpha: %d")) {
        didValueChange = true;
        color->a = clampi(0, alpha, 3);
    }
    igEndPopup();
    igPopID();
    return didValueChange;
}

bool textureWindow_printColorGenerationParameter(TexGenerationContext* generationContext, const TexGeneratorParameterInfo* paramInfo)
{
    bool shouldRegenerate = false;
    GColor value = texgen_getParamColor(generationContext, paramInfo->id);

    if (igcGColorPicker(paramInfo->description, &value)) {
        shouldRegenerate = true;
        texgen_setParamColor(generationContext, paramInfo->id, value);
    }
    igSameLine(0.0f, -1.0f);
    igTextUnformatted(paramInfo->name, NULL);
    return shouldRegenerate;
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
    bool shouldRegenerate =
        textureWindow_printGenerators(generationContext) ||
        textureWindow_printGenerationSize(generationContext);
    igSeparator();
    shouldRegenerate = shouldRegenerate || textureWindow_printGenerationParameters(generationContext);

    if (shouldRegenerate) {
        texgen_execute(generationContext);
        me->uploadedTextureIndex = -1;
    }
}

static bool isTextureAlreadyImported(TextureManager* manager, const char* filename)
{
    const int count = textureManager_getTextureCount(manager);
    for (int i = 0; i < count; i++) {
        const Texture* texture = textureManager_getTextureByIndex(manager, i);
        const char* source = textureManager_getTextureSource(manager, texture);
        if (strcmp(source, filename) == 0)
            return true;
    }
    return false;
}

void textureWindow_printImportFromFileMenu(TextureWindow* me)
{
    if (igMenuItemBool("Import from file", "", false, true)) {
        igOpenPopup("filePicker");
        if (me->textureFiles != NULL)
            stb_readdir_free(me->textureFiles);
        me->textureFiles = stb_readdir_files_mask(TEXTURE_PATH, "*.png");
    }
    if (!igBeginPopup("filePicker", ImGuiWindowFlags_None))
        return;
    bool didAddSomething = false;
    for (int i = 0; i < stb_arr_len(me->textureFiles); i++) {
        const char* filename = me->textureFiles[i] + strlen(TEXTURE_PATH);
        if (isTextureAlreadyImported(me->manager, filename))
            continue;
        didAddSomething = true;
        igPushIDInt(i);
        if (igMenuItemBool(filename, "", false, true)) {
            textureManager_registerFile(me->manager, filename);
            me->curTextureIndex = textureManager_getTextureCount(me->manager) - 1;
        }
        igPopID();
    }
    if (!didAddSomething)
        igMenuItemBool("no textures found", "", false, false);
    igEndPopup();
}

void textureWindow_printGenerateNewMenu(TextureWindow* me)
{
    if (!igMenuItemBool("Generate new", "", false, true))
        return;
    TexGeneratorInfo generatorInfo;
    texgen_getGeneratorInfoByIndex(&generatorInfo, 0);
    TexGenerationContext* texgenctx = textureManager_createGeneratedTexture(me->manager, generatorInfo.id, 64);
    texgen_execute(texgenctx);
    me->curTextureIndex = textureManager_getTextureCount(me->manager) - 1;
}

Window* textureWindow_asWindow(TextureWindow* me)
{
    return me->window;
}
