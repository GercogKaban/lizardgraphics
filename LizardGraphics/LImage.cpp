#include "pch.h"
#include "LImage.h"
#include "LLogger.h"

#include "LResourceManager.h"

namespace LGraphics
{
#ifdef MEGATEXTURE_LG
    void LImage::setDiffuse(const char* path)
    {
        this->texturePath = std::filesystem::current_path().generic_string() + "/" + path;
    }
#else
    void LImage::setDiffuse(GLuint id)
    { 
        ((LResourceManager::OpenGLImage*)textures)->diffuse = id;
    }
    void LImage::setDiffuse(VkImageView view)
    { 
        //imageType = 1;
        //((LResourceManager::VulkanImage*)textures.get())->diffuse.texture = view;
    }
#endif

    LImage::LImage(const char* path, RenderingAPI api)
    {
        texturesType = api;
        texturePath = !path?"": std::filesystem::current_path().generic_string() + "/" + path;
        if (!texturePath.size()) turnOffTexture();
        init();
    }

    void LImage::bindDiffuse(const char* path)
    {
        turnOnTexture();
        texturePath = path;
        //textures = resManager.loadTexture(path,mipLevels);
    }

    //void LImage::bindTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel)
    //{
    //    turnOnTexture();
    //    texture = resManager.loadTexture(bytes,size, name, mipLevels);
    //}

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
            textures = resManager.loadTexture(texturePath.data(),mipLevels);
        //else if (imageType == 0)
        //    texture = resManager.loadTexture(texturesBytes, texturesBytesSize);
    }

    LImage::~LImage()
    {
        //if (imageType == 1)
        //{
        //    if (texturesBytes) delete[] texturesBytes;
        //    if (texture) delete texture;
        //}
    }
}