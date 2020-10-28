#include "pch.h"
#include "LResourceManager.h"
#include "LError.h"
#include "textures.h"

namespace LGraphics
{
    std::map<std::string, GLuint> LResourceManager::textures;

    GLuint LResourceManager::loadTexture(const char* path)
    {
        int width, height;
        unsigned char* image;

        if (!path)
           image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
        else
        {
            if (textures.find(path) != textures.end())
                return textures[path];

            image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
            if (!image)
            {
                image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
                LWRITE("item at address: ", path, " is missing"), LPRINT();
            }
        }
        GLuint texture = initTexture(image, width, height);
        if (path && image) textures.insert(std::make_pair(path, texture));
        return texture;
        //if (texturePath.size()) texturePath.clear();
        //if (texturesBytes) delete texturesBytes;
    }

    GLuint LResourceManager::loadTexture(const unsigned char* bytes, size_t size)
    {
        int width, height;
        auto image = SOIL_load_image_from_memory(bytes, size, &width, &height, 0, SOIL_LOAD_RGBA);
        return initTexture(image, width, height);
    }

    GLuint LResourceManager::initTexture(unsigned char* bytes, int width, int height)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }
}
