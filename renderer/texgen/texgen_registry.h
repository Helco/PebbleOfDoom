/* In this file are the registered texture generators
 * listed, DO NOT put any other code in here!
 * For a new generator, remember to declare the header
 * as well as the symbol in the texgen_registry!
 */

extern TEXGENERATOR_HEADER(XOR);

static const TexGeneratorInitializer rawtexgen_registry[] = {
    TEXGENERATOR_SYMBOL(XOR)
};
