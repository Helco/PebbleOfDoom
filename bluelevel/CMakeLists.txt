###################################################################
# BlueLevel (library for level editor)
###################################################################

set(sources_bluelevel
    bluelist.h
    bluelist.c

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

