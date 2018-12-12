###################################################################
# PoD renderer
###################################################################

set(sources_podrenderer
    renderer.h
    renderer.cpp
    renderer_debug.c
    algebra_float.c
    algebra_float.cpp
    algebra.h
    algebra.cpp
    level.h
    level.c

    texgen/texgen.h
    texgen/texgen_internal.h
    texgen/texgen_registry.h
    texgen/texgen.c
    texgen/texgen_xor.c
    texgen/texgen_rand.c

    ../pcmockup/pebble.c
)
assign_source_group(${sources_podrenderer})

add_library(podrenderer ${sources_podrenderer})
target_include_directories(podrenderer
    INTERFACE "."
    PRIVATE "../pcmockup/" # to access pebble.h
    PRIVATE ${SDL2_INCLUDE_DIR} # for debug output
)
target_include_directories(podrenderer SYSTEM
    PUBLIC "nostl"
)
target_compile_definitions(podrenderer
    PUBLIC REAL_USE_FLOAT
    PUBLIC DEBUG_WINDOWS
)
target_compile_options(podrenderer
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)
target_link_libraries(podrenderer
    PUBLIC cimgui
    PRIVATE nostl
)
enable_warnings(podrenderer)
