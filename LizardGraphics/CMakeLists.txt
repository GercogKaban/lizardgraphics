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
CodeGen.cpp FileSystemDialog.cpp LCircle.cpp imgui/imgui.cpp LCube.cpp LCubeBuffer.cpp LSkyBox.cpp)

set (IMGUI_SOURCE_FILES imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_vulkan.cpp imgui/imgui.cpp 
imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)

set (HEADER_FILES pch.h textures.h Lshaders.h vectors.h
LShape.h LRectangleShape.h LRectangleBuffer.h 
LImage.h LLogger.h LBuffer.h 
LApp.h LWidget.h LTimer.h additional.h LMultiRectangleBuffer.h 
LMultiWRectangle.h LWRectangle.h LResourceManager.h 
ObjectPool.h LModel.h LModelBuffer.h
CodeGen.h FileSystemDialog.h LCircle.h LCube.h LCubeBuffer.h LSkyBox.h)

set (IMGUI_HEADER_FILES imgui/imgui.h imgui/imconfig.h imgui/backends/imgui_impl_glfw.h 
imgui/backends/imgui_impl_vulkan.h imgui/imgui_internal.h imgui/imstb_rectpack.h 
imgui/imstb_textedit.h imgui/imstb_truetype.h)

set(TINYOBJ include/tinyobjloader/tiny_obj_loader.h)

add_library(LizardGraphics STATIC ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES} ${TINYOBJ})
target_include_directories(LizardGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(vulkan/vulkan.h PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC imgui imgui/backends include)

source_group("imgui_headers" FILES ${IMGUI_HEADER_FILES})
source_group("imgui_source" FILES ${IMGUI_SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

add_dependencies(LizardGraphics Optimized)

target_link_libraries(LizardGraphics Optimized)
target_precompile_headers(LizardGraphics PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:pch.h>")
target_compile_features(LizardGraphics PRIVATE cxx_std_17)
