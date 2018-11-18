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
)
assign_source_group(${sources_pcmockup})

add_executable(pcmockup ${sources_pcmockup})
target_link_libraries(pcmockup
    ${SDL2_LIBRARY}
    ${STB_LIBRARY}
    podrenderer
)
enable_warnings(pcmockup)
