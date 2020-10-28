#pragma once

#include <map>
#include "include/GLEW/glew.h"

namespace LGraphics
{
    class LResourceManager
    {
    public:

        static GLuint loadTexture(const char* path);
        static GLuint loadTexture(const unsigned char* bytes, size_t size);

    private:

        static GLuint initTexture(unsigned char* bytes, int width, int height);

        static std::map<std::string, GLuint> textures;
    };
}

