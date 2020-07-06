#include "LImage.h"
#include "include/SOIL2/SOIL2.h"
#include "textures.h"
#include "LError.h"
#include "pch.h"

namespace LGraphics
{
    LImage::LImage(const char* path)
    {
        bindTexture(path);
    }

    LImage::LImage(const unsigned char* bytes, size_t size)
    {
        bindTexture(bytes, size);
    }

    void LImage::bindTexture(const char* path)
    {
        int width, height;
        unsigned char* image;

        if (!path)
            image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
        else
        {
            image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
            if (!image)
            {
                image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
                LWRITE("item at address: ", path, " is missing"), LPRINT();
            }
        }
        bindTexture(image, width, height);
    }

    void LImage::bindTexture(const unsigned char* bytes, size_t size)
    {
        int width, height;
        auto image = SOIL_load_image_from_memory(bytes, size, &width, &height, 0, SOIL_LOAD_RGBA);
        bindTexture(image, width, height);
    }

    void LImage::bindTexture(unsigned char* bytes, int width, int height)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LImage::turnOffTexture()
    {
        turnedOn = false;
    }

    void LImage::turnOnTexture()
    {
        turnedOn = true;
    }

    void LImage::switchTexture(bool show)
    {
        turnedOn = show;
    }

    void LImage::switchTexture()
    {
        turnedOn = !turnedOn;
    }
    bool LImage::isTextureTurnedOn() const
    {
        return turnedOn;
    }
}