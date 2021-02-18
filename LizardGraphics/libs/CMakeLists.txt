message("-- Linking SDL")
add_library(SDL2 SDL2.dll SDL2.lib SDL2main.lib SDL2-static.lib)
set_target_properties(SDL2 PROPERTIES LINKER_LANGUAGE C)