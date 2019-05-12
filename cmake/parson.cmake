# Configures parson for use in pcmockup

set(PARSON_DIR "${CMAKE_SOURCE_DIR}/external/parson/")

set(sources_parson
    ${PARSON_DIR}/parson.h
    ${PARSON_DIR}/parson.c
)

add_library(parson ${sources_parson})
target_include_directories(parson PUBLIC ${PARSON_DIR})
