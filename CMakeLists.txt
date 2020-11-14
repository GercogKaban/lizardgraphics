cmake_minimum_required (VERSION 3.16)
     
project (LS_Example C CXX)

find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp CodeGen.cpp)
set(HEADER_FILES CodeGen.h)

ADD_EXECUTABLE(LS_Example ${SOURCE_FILES} ${HEADER_FILES})
add_subdirectory (LizardGraphics)

target_precompile_headers(LS_Example PRIVATE pch.h)

IF (WIN32)
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32s LizardGraphics/libs/freetype)
ELSE()
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32 LizardGraphics/libs/freetype)
ENDIF()