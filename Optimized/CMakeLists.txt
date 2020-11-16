cmake_minimum_required (VERSION 3.16)

project (Optimized)
set(MY_BCP ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Release)

set (SOURCE_FILES optimized.h optimized.cpp)
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(Optimized SHARED ${SOURCE_FILES})
target_include_directories(Optimized PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set_target_properties(Optimized PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) #LIBRARY_OUTPUT_DIRECTORY

set(CMAKE_BUILD_TYPE ${MY_BCP})