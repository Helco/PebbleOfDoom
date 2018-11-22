###################################################################
# PC Mockup
###################################################################

set(sources_pcmockup
    pebble.h
    sdl.include.h
    stb.include.h
    pcmockup.h
    pcmockup.c
    pebblewindow.c
    debugwindow.c
    debugwindowset.c
    windowgrid.c
    safeframebuffer.c
)
assign_source_group(${sources_pcmockup})

add_executable(pcmockup ${sources_pcmockup})
target_link_libraries(pcmockup
    ${SDL2_LIBRARY}
    ${STB_LIBRARY}
    podrenderer
)
target_include_directories(pcmockup
    PRIVATE ${SDL2_INCLUDE_DIR}
)
enable_warnings(pcmockup)
