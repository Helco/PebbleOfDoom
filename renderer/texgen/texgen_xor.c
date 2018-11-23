#include "texgen_internal.h"

typedef struct TexGen_XOR_Params {
    bool_t useColor;
} TexGen_XOR_Params;

static const TexGen_XOR_Params texGen_xor_defaultParams = {
    .useColor = true
};
_Bool b;

bool_t texGen_xor_execute(GColor* output, int logSize, const void* rawParams)
{
    const TexGen_XOR_Params* params = (const TexGen_XOR_Params*)rawParams;
    int size = 1 << logSize;
    int pixelCount = size * size;
    for (int i = 0; i < pixelCount; i++, output++)
    {
        int x = i & (size - 1);
        int y = i >> logSize;
        int c = x ^ y;
        output->a = 3;
        output->r = output->g = output->b = c & 3;
        if (params->useColor)
        {
            output->g = (c >> 2) & 3;
            output->b = (c >> 2) & 3;
        }
    }
    return true;
}

BEGIN_TEXGENERATOR(XOR, TexGen_XOR_Params)
    TEXGENPARAM_BOOL(
        TexGenParam_XOR_UseColor,
        "Color or black/white",
        useColor)
END_TEXGENERATOR(XOR,
    TexGenerator_XOR, "XOR textures",
    texGen_xor_defaultParams,
    texGen_xor_execute)

