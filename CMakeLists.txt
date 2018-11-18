cmake_minimum_required (VERSION 3.9.2)
project(pebbleofdoom)

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

# To enable many many warnings and treat them as errors (portable across msvc/gcc)
function(enable_warnings TARGET)
    if (MSVC)
        target_compile_options(${TARGET} PRIVATE "/W4" "/WX")
    else()
        target_compile_options(${TARGET} PRIVATE
            "-Werror" "-Wall" "-Wextra"
            "-Wpointer-arith" "-Wlogical-op"
            "-Wdisabled-optimization" "-Wunsafe-loop-optimizations"
        )
    endif()
endfunction(enable_warnings)

###################################################################
# Dependencies
###################################################################

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/" ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    link_libraries(m)
endif()

include(CTest)
include(GoogleTest)
include("${CMAKE_SOURCE_DIR}/cmake/GTest.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/stb.cmake")

if (MSVC AND CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR " Microsoft C Compiler is not supported, please use GCC or Clang")
endif()

###################################################################
# PoD renderer
###################################################################

set(sources_podrenderer
    renderer/renderer.h
    renderer/renderer.c
    renderer/renderer_debug.c
    renderer/algebra_float.c
    renderer/algebra.h
    renderer/algebra.c

    pcmockup/pebble.c
)
assign_source_group(${sources_podrenderer})

add_library(podrenderer ${sources_podrenderer})
target_include_directories(podrenderer PUBLIC
    "pcmockup/" # to access pebble.h
    ${SDL2_INCLUDE_DIR} # for debug output
)
target_compile_definitions(podrenderer
    PUBLIC REAL_USE_FLOAT
    PUBLIC DEBUG_WINDOWS
)
enable_warnings(podrenderer)

###################################################################
# tests
###################################################################

set(sources_test_podrenderer
    test/fixtures.h
    test/test_real.cpp
    test/test_vector.cpp
    test/test_intersection.cpp
    test/test_integer.cpp
)
assign_source_group(${sources_test_podrenderer})

add_executable(test_podrenderer ${sources_test_podrenderer})
target_link_libraries(test_podrenderer
    podrenderer
    gtest_main
)
add_googletest(test_podrenderer "def.")

###################################################################
# PC Mockup
###################################################################

set(sources_pcmockup
    pcmockup/pebble.h
    pcmockup/sdl.include.h
    pcmockup/stb.include.h
    pcmockup/stb_impl.c

    pcmockup/pcmockup.h
    pcmockup/pcmockup.c
    pcmockup/pebblewindow.c
    pcmockup/debugwindow.c
    pcmockup/debugwindowset.c
    pcmockup/windowgrid.c
)
assign_source_group(${sources_pcmockup})

add_executable(pcmockup ${sources_pcmockup})
target_link_libraries(pcmockup
    ${SDL2_LIBRARY}
    ${STB_LIBRARY}
    podrenderer
)
target_include_directories(pcmockup PUBLIC
    ${STB_INCLUDE_DIR}
    "renderer/"
)
enable_warnings(pcmockup)
