#include "texgen_internal.h"

typedef struct TexGen_SolidColor_Params {
    GColor color;
} TexGen_SolidColor_Params;

static const TexGen_SolidColor_Params texGen_solidColor_defaultParams = {
    .color = { .r = 3, .g = 3, .b = 3, .a = 3 }
};

bool_t texGen_solidColor_execute(GColor* output, int logSize, const void* rawParams)
{
    const TexGen_SolidColor_Params* params = (const TexGen_SolidColor_Params*)rawParams;
    int size = 1 << logSize;
    int pixelCount = size * size;
    memset(output, params->color.argb, pixelCount);
    return true;
}

BEGIN_TEXGENERATOR(SolidColor, TexGen_SolidColor_Params)
    TEXGENPARAM_COLOR(
        TexGenParam_SolidColor_Color,
        "color",
        color)
END_TEXGENERATOR(SolidColor,
    TexGenerator_SolidColor,
    "single-color filled texture",
    texGen_solidColor_defaultParams,
    texGen_solidColor_execute)

