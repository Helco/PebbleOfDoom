# Configures glad for use in PebbleOfDoom

set(GLAD_DIR "${CMAKE_SOURCE_DIR}/external/glad")
if (NOT EXISTS "${GLAD_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Could not find glad, did you clone the submodules?")
endif()

set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_API "gl=3.0" CACHE STRING "" FORCE)
set(GLAD_GENERATOR "c" CACHE STRING "" FORCE)
set(GLAD_EXTENSIONS "" CACHE STRING "" FORCE)
set(GLAD_SPEC "gl" CACHE STRING "" FORCE)
option(GLAD_NO_LOADER "" ON)
add_subdirectory(${GLAD_DIR})
