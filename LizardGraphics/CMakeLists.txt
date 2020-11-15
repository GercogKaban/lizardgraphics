cmake_minimum_required (VERSION 3.16)

project (LizardGraphics)

add_definitions(-DGLEW_STATIC)

set (SOURCE_FILES textures.cpp LTextBuffer.cpp LText.cpp LTextEdit.cpp LShape.cpp LRectangleShape.cpp LIRectangle.cpp LImage.cpp LIButton.cpp LScroller.cpp LRectangleBuffer.cpp LError.cpp LBuffer.cpp LApp.cpp LWidget.cpp LLine.cpp LTimer.cpp additional.cpp LWRectangle.cpp LResourceManager.cpp LFlag.cpp LCounter.cpp LMessageBox.cpp LColorBar.cpp)
set (HEADER_FILES pch.h textures.h Lshaders.h vectors.h LTextBuffer.h LText.h LTextEdit.h LShape.h LRectangleShape.h LRectangleBuffer.h LIRectangle.h LScroller.h LImage.h LIButton.h LError.h LBuffer.h LApp.h LWidget.h LLine.h LTimer.h additional.h LWRectangle.h LResourceManager.h LFlag.h LCounter.h LMessageBox.h LColorBar.h ObjectPool.h)

add_library(LizardGraphics STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(LizardGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(LizardGraphics/freetype PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(LizardGraphics/ft2build.h PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_precompile_headers(LizardGraphics PUBLIC pch.h)