cmake_minimum_required (VERSION 3.16)
     
project (LS_Example C CXX)

set(API "OPENGL" CACHE STRING "RENDERING API")
add_compile_definitions(${API})
#message (${API})

IF (${API} MATCHES "OPENGL")
find_package(OpenGL REQUIRED)
ENDIF (${API} MATCHES "OPENGL")

set(SOURCE_FILES CodeGen.cpp main.cpp)
set(HEADER_FILES CodeGen.h)

ADD_EXECUTABLE(LS_Example ${SOURCE_FILES} ${HEADER_FILES})
add_subdirectory (LizardGraphics)

target_precompile_headers(LS_Example PRIVATE pch.h)

IF (${API} MATCHES "VULKAN")
   target_link_libraries(LS_Example LizardGraphics LizardGraphics/libs/vulkan-1) 
   add_custom_command(TARGET LS_Example PRE_BUILD COMMAND python build_shaders.py)
ENDIF(${API} MATCHES "VULKAN")

IF (WIN32)
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32s LizardGraphics/libs/freetype)
ELSE()
  target_link_libraries(LS_Example LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32 LizardGraphics/libs/freetype)
ENDIF()

target_compile_features(LS_Example PRIVATE cxx_std_17)