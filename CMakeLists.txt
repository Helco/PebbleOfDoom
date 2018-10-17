cmake_minimum_required (VERSION 3.10.2)
project(pebbleofdoom)

###################################################################
# Dependencies
###################################################################

find_package(SDL2 REQUIRED)
set(CMAKE_C_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    link_libraries(m)
endif()

###################################################################
# Utilities
###################################################################

# To preserve file hierarchy in Visual Studio
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

###################################################################
# PoD renderer
###################################################################

set(sources_podrenderer
    renderer/renderer.h
    renderer/renderer.c
)
assign_source_group(${sources_podrenderer})

add_library(podrenderer ${sources_podrenderer})
target_include_directories(podrenderer PUBLIC
    "pcmockup/" # to access pebble.h
)

###################################################################
# PC Mockup
###################################################################

set(sources_pcmockup
    pcmockup/pebble.h
    pcmockup/pebble.c
    pcmockup/stb_impl.c

    pcmockup/pcmockup.h
    pcmockup/pcmockup.c
)
assign_source_group(${sources_pcmockup})

add_executable(pcmockup ${sources_pcmockup})
add_dependencies(pcmockup
    podrenderer
)
target_link_libraries(pcmockup
    ${SDL2_LIBRARIES}
    podrenderer
)
target_include_directories(pcmockup PUBLIC
    ${SDL2_INCLUDE_DIRS}
    "external/stb/"
    "renderer/"
)
