#include "pch.h"
#include "LImage.h"
#include "LError.h"

#include "LResourceManager.h"

namespace LGraphics
{
    LImage::LImage(const char* path)
    {
        //if (!lazy)
            //texture = resManager.loadTexture(path);
        texturePath = !path?"":path;
        if (!texturePath.size()) turnOffTexture();
        init();
    }

#ifdef OPENGL
    LImage::LImage(const unsigned char* bytes, size_t size)
    {
        //if (!lazy)
            //texture = resManager.loadTexture(bytes,size);
        //else
        {
            texturesBytes = new unsigned char[size];
            memcpy(texturesBytes, bytes, size);
            texturesBytesSize = size;
            //init();
        }
    }
#endif

    void LImage::bindTexture(const char* path, int desiredChannel)
    {
        turnOnTexture();
        texturePath = path;
        texture = resManager.loadTexture(path);
    }

    void LImage::bindTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel)
    {
        turnOnTexture();
        texture = resManager.loadTexture(bytes,size, name, desiredChannel);
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

    void LImage::init()
    {
        if (texturePath.size())
            texture = resManager.loadTexture(texturePath.data());
#ifdef OPENGL
        else 
            texture = resManager.loadTexture(texturesBytes, texturesBytesSize);
#endif
    }

    LImage::~LImage()
    {
#ifdef OPENGL
        if (texturesBytes) delete[] texturesBytes;
#endif
    }
}