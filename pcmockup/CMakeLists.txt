###################################################################
# PC Mockup
###################################################################

set(sources_pcmockup
    pebble.h
    sdl.include.h
    stb.include.h
    cimgui.include.h

    window.h
    window_internal.h
    window.c
    windowcontainer.c

    pcmockup.h
    pcmockup.c
    pebblewindow.c
    debugwindow_sdl.c
    debugwindow_imgui.c
    windowgrid.c
    safeframebuffer.c
    rendererinterface.c
    texturemanager.c
    levelmanager.c
    windowcontainer.c
    imagewindow.c
    texturewindow.c
    uploadedtexture.c
)
assign_source_group(${sources_pcmockup})

add_executable(pcmockup ${sources_pcmockup})
target_link_libraries(pcmockup
    ${SDL2_LIBRARY}
    stb
    podrenderer
    parson
)
target_include_directories(pcmockup
    PRIVATE ${SDL2_INCLUDE_DIR}
)
set_target_properties(pcmockup PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
enable_warnings(pcmockup)
add_sanitizers(pcmockup)
