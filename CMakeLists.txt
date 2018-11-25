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
            "-Wc++-compat"
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
include("${CMAKE_SOURCE_DIR}/cmake/glad.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/cimgui.cmake")

if (MSVC AND CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR " Microsoft C Compiler is not supported, please use GCC or Clang")
endif()

###################################################################
# sub-directories
###################################################################

add_subdirectory("renderer")
add_subdirectory("test")
add_subdirectory("pcmockup")
add_subdirectory("tools")
