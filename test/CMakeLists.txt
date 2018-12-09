###################################################################
# tests
###################################################################
if (EMSCRIPTEN)
  return()
endif()

set(sources_test_podrenderer
    fixtures.h
    test_real.cpp
    test_vector.cpp
    test_intersection.cpp
    test_integer.cpp
    test_lineSeg.cpp
)
assign_source_group(${sources_test_podrenderer})

add_executable(test_podrenderer ${sources_test_podrenderer})
target_link_libraries(test_podrenderer
    podrenderer
    gtest_main
)
target_include_directories(test_podrenderer PUBLIC
    "../renderer/"
)
add_googletest(test_podrenderer "def.")
