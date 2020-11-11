#include "pch.h"
#include "LImage.h"
#include "textures.h"
#include "LError.h"

namespace LGraphics
{
    LImage::LImage(const char* path)
    {
        //if (!lazy)
            //texture = resManager.loadTexture(path);
        texturePath = !path?"":path;
        if (!texturePath.size()) turnOffTexture();
        //init();
    }

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

    void LImage::bindTexture(const char* path)
    {
        turnOnTexture();
        texturePath = path;
        texture = resManager.loadTexture(path);
    }

    void LImage::bindTexture(const unsigned char* bytes, size_t size)
    {
        //turnOnTexture();
        texture = resManager.loadTexture(bytes,size);
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
        else 
            texture = resManager.loadTexture(texturesBytes, texturesBytesSize);
    }

    LImage::~LImage()
    {
        if (texturesBytes) delete[] texturesBytes;
    }
}