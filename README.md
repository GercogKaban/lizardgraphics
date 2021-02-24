# lizardgraphics
C++ Graphic library written on OpenGL and Vulkan API.
As GUI Lizard Graphics offering Dear ImGui lib.

Reasons to use:

1. Fast primitives renderering.
2. Сross-platform.
3. Free.

# The necessary libraries

glfw - https://www.glfw.org/download.html

glew32 - http://glew.sourceforge.net/ or

vulkan-1 - https://www.lunarg.com/vulkan-sdk/

On Windows you need to place this static libraries in the "LizardGraphics/libs" directory.

# Building for Windows

```sh
git clone https://github.com/GercogKaban/lizardgraphics
cd /d path
cmake -G "Visual Studio 16 2019" -A x64 -S path -DAPI=VULKAN
``` 

# Building for Linux

```sh
git clone https://github.com/GercogKaban/lizardgraphics
cd path
cmake . -DAPI=VULKAN
make
```

also you can choose OpenGL api: -DAPI=OPENGL (currently unavalaible :) )

# TODO

## Features
1. Antialiasing
2. Shadows.
3. Reflections.
4. Supporting Android platform.
5. Runtime API changing.
6. Skeletal animation.
7. Grid drawing.

## Optimizations

1. Multithread command buffers writing (Vulkan)
2. Prerecording command buffers (Vulkan).

# Documentation
Here you can find documentation for Lizard Graphics https://gercogkaban.github.io/lizardgraphics/index.html
