# lizardgraphics
Graphic widget library written on C++ and OpenGL functions.

Reasons to use:

1. Fast widgets renderering.
2. Сross-platform.
3. Free.

# The necessary libraries

glfw - https://www.glfw.org/download.html

glew32 - http://glew.sourceforge.net/

SOIL2 - https://github.com/SpartanJ/SOIL2

Also you can find all of this libraries here: https://drive.google.com/open?id=1xMj5cn3cL8jD9MrSl9WjcWl4D283tu6g

You need to download and place this static libraries in the "libs" directory.

# Building for Windows
1. Open command line
2. Go to the repository directory (cd /d path)
3. Input following commands:

cmake -G "Visual Studio 16 2019" -A x64 -S path -B "build64"
cmake --build build64 --config Debug

# Building for Linux
Coming soon...
