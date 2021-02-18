cmake_minimum_required (VERSION 3.16)

project (LizardGraphics)

add_definitions(-DGLEW_STATIC)

add_subdirectory (../Optimized Optimized)

set(API "OPENGL" CACHE STRING "RENDERING API")
add_compile_definitions(${API})

set (SOURCE_FILES textures.cpp LShape.cpp LRectangleShape.cpp LImage.cpp 
LRectangleBuffer.cpp LLogger.cpp LBuffer.cpp LApp.cpp LWidget.cpp 
LTimer.cpp additional.cpp LMultiRectangleBuffer.cpp LMultiWRectangle.cpp 
LWRectangle.cpp LResourceManager.cpp Lshaders.cpp LModel.cpp LModelBuffer.cpp
CodeGen.cpp FileSystemDialog.cpp)

set (IMGUI_SOURCE_FILES imgui_impl_glfw.cpp imgui_impl_vulkan.cpp imgui.cpp imgui_demo.cpp 
imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_impl_sdl.cpp)

set (HEADER_FILES pch.h textures.h Lshaders.h vectors.h
LShape.h LRectangleShape.h LRectangleBuffer.h 
LImage.h LLogger.h LBuffer.h 
LApp.h LWidget.h LTimer.h additional.h LMultiRectangleBuffer.h 
LMultiWRectangle.h LWRectangle.h LResourceManager.h ObjectPool.h
imgui.h imconfig.h imgui_impl_glfw.h imgui_impl_vulkan.h imgui_internal.h
imstb_rectpack.h imstb_textedit.h imstb_truetype.h LModel.h LModelBuffer.h
CodeGen.h FileSystemDialog.h)

set (IMGUI_HEADER_FILES imgui.h imconfig.h imgui_impl_glfw.h imgui_impl_vulkan.h imgui_internal.h
imstb_rectpack.h imstb_textedit.h imstb_truetype.h imgui_impl_sdl.h)

set(TINYOBJ include/tinyobjloader/tiny_obj_loader.h)

add_library(LizardGraphics STATIC ${HEADER_FILES} ${SOURCE_FILES} 
${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES} ${TINYOBJ})
target_include_directories(LizardGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(imgui.h PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(vulkan/vulkan.h PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SDL2-2.0.14/include)

source_group("imgui_headers" FILES ${IMGUI_HEADER_FILES})
source_group("imgui_source" FILES ${IMGUI_SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})
#IF(${API} MATCHES "VULKAN")
    #add_compile_definitions(GLFW_INCLUDE_VULKAN)
    #include_directories(LizardGraphics/include/Vulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#ENDIF(${API} MATCHES "VULKAN")

add_dependencies(LizardGraphics Optimized)

target_link_libraries(LizardGraphics Optimized)

target_precompile_headers(LizardGraphics PUBLIC pch.h)
target_compile_features(LizardGraphics PRIVATE cxx_std_17)
