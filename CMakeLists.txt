cmake_minimum_required (VERSION 2.8)
     
project (LS_Example CXX)

find_package(OpenGL REQUIRED)

add_executable(LS_Example main.cpp)
add_subdirectory (LizardGraphics)

target_link_libraries(LS_Example LizardGraphics opengl32 ../libs/glfw3 ../libs/soil2-debug ../libs/glew32s ../libs/glew32)