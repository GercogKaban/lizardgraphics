cmake_minimum_required (VERSION 3.16)
     
set(API "OPENGL" CACHE STRING "RENDERING API")
set(PLATFORM "DESKTOP" CACHE STRING "PLATFORM YOU TARGETING")
add_compile_definitions(${API})

IF (${API} MATCHES "OPENGL")
find_package(OpenGL REQUIRED)
ENDIF (${API} MATCHES "OPENGL")

IF (${PLATFORM} MATCHES "DESKTOP")
project (DesktopExample C CXX)
set(SOURCE_FILES main.cpp)
set(HEADER_FILES)

ADD_EXECUTABLE(DesktopExample ${SOURCE_FILES} ${HEADER_FILES})
add_subdirectory (LizardGraphics)

target_precompile_headers(DesktopExample PRIVATE pch.h)

IF (${API} MATCHES "VULKAN")
   #target_link_libraries(DesktopExample LizardGraphics LizardGraphics/libs/vulkan-1)
IF(WIN32)
   #add_custom_command(TARGET DesktopExample PRE_BUILD COMMAND python build_shaders.py)
ENDIF(WIN32)
ENDIF(${API} MATCHES "VULKAN")

IF (WIN32)
  #target_link_libraries(DesktopExample LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/glew32s)
ELSE()
  #target_link_libraries(DesktopExample LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/glew32)
ENDIF()

target_compile_features(DesktopExample PRIVATE cxx_std_17)
ENDIF (${PLATFORM} MATCHES "DESKTOP")

IF(${PLATFORM} MATCHES "MOBILE")
project (AndroidSharedLib C CXX)

set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++11 \
                     -DVK_USE_PLATFORM_ANDROID_KHR")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(AndroidSharedLib SHARED dllmain.h dllmain.cpp vulkan_wrapper.cpp)
target_link_libraries(AndroidSharedLib LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/glew32s)
target_link_libraries(AndroidSharedLib LizardGraphics LizardGraphics/libs/vulkan-1 app-glue log android) 
target_compile_features(AndroidSharedLib PRIVATE cxx_std_17)

ENDIF(${PLATFORM} MATCHES "MOBILE")