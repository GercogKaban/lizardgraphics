cmake_minimum_required (VERSION 2.8)

project (LizardGraphics)

add_definitions(-DGLEW_STATIC)

set (SOURCE_FILES pch.cpp textures.cpp LTextBuffer.cpp LText.cpp LTextEdit.cpp LShape.cpp LRectangleShape.cpp LIRectangle.cpp LImage.cpp LIButton.cpp LRectangleBuffer.cpp LError.cpp LBuffer.cpp LApp.cpp LWidget.cpp LLine.cpp LTimer.cpp additional.cpp LWRectangle.cpp LResourceManager.cpp)
set (HEADER_FILES pch.h textures.h Lshaders.h vectors.h LTextBuffer.h LText.h LTextEdit.h LShape.h LRectangleShape.h LRectangleBuffer.h LIRectangle.h LImage.h LIButton.h LError.h LBuffer.h LApp.h LWidget.h LLine.h LTimer.h additional.h LWRectangle.h LResourceManager.h)

add_library(LizardGraphics STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(LizardGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(LizardGraphics/freetype PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(LizardGraphics/ft2build.h PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})