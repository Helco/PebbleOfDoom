cmake_minimum_required (VERSION 3.9.2)
project(pebbleofdoom)

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

if (MSVC AND CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR " Microsoft C Compiler is not supported, please use GCC or Clang")
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

# To add googletests in the best way regarding cmake version
function(add_googletest TARGET PREFIX)
    if (CMAKE_VERSION VERSION_LESS "3.10.0")
        gtest_add_tests(TARGET ${TARGET} TEST_PREFIX ${PREFIX})
    else()
        gtest_discover_tests(${TARGET} TEST_PREFIX ${PREFIX})
    endif()
endfunction(add_googletest)

###################################################################
# googletest
###################################################################

configure_file(external/GTest.CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
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

###################################################################
# tests
###################################################################

set(sources_test_podrenderer
    test/fixtures.h
    test/test_real.cpp
    test/test_vector.cpp
    test/test_intersection.cpp
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
add_dependencies(pcmockup
    podrenderer
)
target_link_libraries(pcmockup
    ${SDL2_LIBRARY}
    podrenderer
)
target_include_directories(pcmockup PUBLIC
    "external/stb/"
    "renderer/"
)
