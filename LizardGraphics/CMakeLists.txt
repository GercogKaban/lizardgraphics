cmake_minimum_required (VERSION 2.8)

project (LizardGraphics)

set (SOURCE_FILES pch.cpp LizardGraphics.cpp textures.cpp)
set (HEADER_FILES pch.h LizardGraphics.h textures.h Lshaders.h vectors.h)

add_library(LizardGraphics STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(LizardGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})