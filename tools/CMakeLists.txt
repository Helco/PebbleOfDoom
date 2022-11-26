###################################################################
# Texture Generator CLI
###################################################################

#add_executable(texgencli
#    "texgencli.c"
#)
#target_link_libraries(texgencli PUBLIC
#    podrenderer
#    stb
#)
#enable_warnings(texgencli)
#add_sanitizers(texgencli)

###################################################################
# Pebble level converter
###################################################################

add_executable(pbllevelconvert
    "pbllevelconvert.c"

    "../pcmockup/levelmanager.c"
)
target_include_directories(pbllevelconvert PRIVATE
    "../pcmockup"
    "../pebbleapp"
)
target_link_libraries(pbllevelconvert PUBLIC
    podrenderer
    stb
    parson
)
enable_warnings(pbllevelconvert)
add_sanitizers(pbllevelconvert)
