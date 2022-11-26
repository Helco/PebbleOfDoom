###################################################################
# PoD renderer
###################################################################

set(sources_podrenderer
    renderer.h
    renderer_internal.h
    renderer.c
    renderer_debug.c
    algebra_float.c
    algebra.h
    algebra.c
    level.h
    level.c
    segame.h
    segame.c
    segame_render.c
    menu.c
    entities.c
    resources.h

    ../pcmockup/pebble.c
)
assign_source_group(${sources_podrenderer})

add_library(podrenderer ${sources_podrenderer})
target_include_directories(podrenderer
    INTERFACE "."
    PRIVATE "../pcmockup/" # to access pebble.h
    PRIVATE ${SDL2_INCLUDE_DIR} # for debug output
)
target_compile_definitions(podrenderer
    PUBLIC REAL_USE_FLOAT
    PUBLIC DEBUG_WINDOWS
)
target_link_libraries(podrenderer
    PUBLIC cimgui
)

enable_warnings(podrenderer)
add_sanitizers(podrenderer)
