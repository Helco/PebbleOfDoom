###################################################################
# PoD renderer
###################################################################

set(sources_podrenderer
    renderer.h
    renderer.c
    renderer_debug.c
    algebra_float.c
    algebra.h
    algebra.c

    ../pcmockup/pebble.c
)
assign_source_group(${sources_podrenderer})

add_library(podrenderer ${sources_podrenderer})
target_include_directories(podrenderer PUBLIC
    "../pcmockup/" # to access pebble.h
    ${SDL2_INCLUDE_DIR} # for debug output
)
target_compile_definitions(podrenderer
    PUBLIC REAL_USE_FLOAT
    PUBLIC DEBUG_WINDOWS
)
enable_warnings(podrenderer)
