###################################################################
# BlueLevel (library for edited level <-> playable level)
###################################################################

set(sources_bluelevel
    blueutil.h
    blueutil.c
    bluelist.h
    bluelist.c
    bluelevel.h
    bluelevel_internal.h
    bluepoint.c
    bluesegment.c
    bluesector.c
    bluelevel.c

    bluelevel_internal.h
)
assign_source_group(${sources_bluelevel})

add_library(bluelevel ${sources_bluelevel})
target_include_directories(bluelevel
    INTERFACE "."
)
target_link_libraries(bluelevel
    PUBLIC podrenderer
    PRIVATE stb
)
enable_warnings(bluelevel)

