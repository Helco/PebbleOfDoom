#include "texgen_internal.h"

typedef struct TexGen_Rand_Params {
    bool_t useColor;
    bool_t useSeed;
    int seed;
} TexGen_Rand_Params;

static const TexGen_Rand_Params texGen_rand_defaultParams = {
    .useColor = true,
    .useSeed = false
};

bool_t texGen_rand_execute(GColor* output, int logSize, const void* rawParams)
{
    const TexGen_Rand_Params* params = (const TexGen_Rand_Params*)rawParams;
    if (params->useSeed)
        srand((unsigned int)params->seed);
    int size = 1 << logSize;
    int pixelCount = size * size;
    for (int i = 0; i < pixelCount; i++, output++)
    {
        int c = rand();
        output->a = 3;
        output->r = output->g = output->b = c & 3;
        if (params->useColor)
        {
            output->g = (c >> 2) & 3;
            output->b = (c >> 4) & 3;
        }
    }
    return true;
}

BEGIN_TEXGENERATOR(Rand, TexGen_Rand_Params)
    TEXGENPARAM_BOOL(
        TexGenParam_Rand_UseColor,
        "color or black/white",
        useColor)
END_TEXGENERATOR(Rand,
    TexGenerator_Rand,
    "random filled texure",
    texGen_rand_defaultParams,
    texGen_rand_execute)
