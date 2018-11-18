# Configures stb for use in PebbleOfDoom

set(STB_DIR "${CMAKE_SOURCE_DIR}/external/stb")
if (NOT EXISTS "${STB_DIR}/stb_image.h")
    message(FATAL_ERROR "Could not find stb_image, did you clone the submodules?")
endif()

set(sources_stb
    pcmockup/stb_impl.c
    ${STB_DIR}/stb_image.h
    ${STB_DIR}/stb_image_write.h
)
assign_source_group(${sources_stb})

add_library(stb ${sources_stb})
target_include_directories(stb PUBLIC
    ${STB_DIR}
    "pcmockup"
)

set(STB_LIBRARY "stb")
set(STB_INCLUDE_DIR ${STB_DIR})
