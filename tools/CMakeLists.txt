###################################################################
# Tools
###################################################################

add_executable(texgencli
    "texgencli.c"
)
target_link_libraries(texgencli PUBLIC
    podrenderer
    stb
)
enable_warnings(texgencli)
add_sanitizers(texgencli)
