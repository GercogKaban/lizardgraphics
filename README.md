# lizardgraphics
C++ Graphic library written on OpenGL and Vulkan API.

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
1. Open command line. 
2. Clone repository (git clone https://github.com/GercogKaban/lizardgraphics).
3. Go to the repository directory (cd /d path)
4. Input following command:

cmake -G "Visual Studio 16 2019" -A x64 -S path -DAPI=VULKAN         (for VS 19).

cmake -G "Visual Studio 15 2017" -A x64 -S path -DAPI=VULKAN         (for VS 17).

also you can choose OpenGL api: -DAPI=OPENGL (currently unavalaible :) )

# Building for Linux
1. Open terminal.
2. Clone repository (git clone https://github.com/GercogKaban/lizardgraphics).
3. Go to the repository directory (cd path)
4. Input following commands:

cmake . -A x64 -DAPI=VULKAN
make

#TODO

1. Model loader.
2. Supporting Android platform.

# Documentation
Here you can find documentation for Lizard Graphics https://gercogkaban.github.io/lizardgraphics/index.html
