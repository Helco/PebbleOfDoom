# Configures cimgui for use in PebbleOfDoom

set(CIMGUI_DIR "${CMAKE_SOURCE_DIR}/external/cimgui/")
set(IMGUI_DIR "${CIMGUI_DIR}/imgui/")
if (NOT EXISTS "${CIMGUI_DIR}/cimgui.h")
    message(FATAL_ERROR "Could not find cimgui, did you clone the submodules?")
endif()
if (NOT EXISTS "${IMGUI_DIR}/imgui.h")
    message(FATAL_ERROR "Could not find imgui, did you clone the submodules *recursively*?")
endif()

set(sources_cimgui
    ${CIMGUI_DIR}/cimgui.h
    ${CIMGUI_DIR}/cimgui.cpp

    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/examples/imgui_impl_sdl.h
    ${IMGUI_DIR}/examples/imgui_impl_sdl.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.h
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

add_library(cimgui ${sources_cimgui})
target_include_directories(cimgui
    PRIVATE ${IMGUI_DIR}
    PRIVATE ${SDL2_INCLUDE_DIR}
    INTERFACE ${CIMGUI_DIR}
    INTERFACE ${IMGUI_DIR}/examples
)
target_link_libraries(cimgui
    PUBLIC ${SDL2_LIBRARY}
    PUBLIC glad
)
target_compile_definitions(cimgui
    PRIVATE "-DIMGUI_IMPL_API=extern \"C\""
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD
    INTERFACE CIMGUI_DEFINE_ENUMS_AND_STRUCTS
)
