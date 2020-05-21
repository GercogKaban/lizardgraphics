cmake_minimum_required (VERSION 2.8)
     
project (LS_Example C CXX)

find_package(OpenGL REQUIRED)

add_executable(LS_Example main.cpp)
add_subdirectory (LizardGraphics)

IF (WIN32)
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32s LizardGraphics/libs/freetype)
ELSE()
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32 LizardGraphics/libs/freetype)
ENDIF()
