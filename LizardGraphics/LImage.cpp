#include "pch.h"
#include "LImage.h"
#include "LLogger.h"
#include "LApp.h"

#include "LResourceManager.h"

namespace LGraphics
{
#ifdef MEGATEXTURE_LG
    void LImage::setDiffuse(const char* path)
    {
        this->imageResourceName = std::filesystem::current_path().generic_string() + "/" + path;
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

    LImage::LImage(ImageResource resource, RenderingAPI api)
    {
        texturesType = api;
        imageResourceName = resource.name;
        if (!imageResourceName.size()) turnOffTexture();
        init(resource);
    }

    LImage::LImage(TexturesData& diffuseData, TexturesData& normalData)
    {
        textures.push_back (new TexturesData(new TexturesData::OGLImageData));
        textures.push_back(new TexturesData(new TexturesData::OGLImageData));

        //LResourceManager::TexturesData& castedInDiffuse = *(LResourceManager::TexturesData*)diffuseData;
        //LResourceManager::TexturesData& castedInNormal = *(LResourceManager::TexturesData*)normalData;

        //TexturesData& castedDiffIn = *(TexturesData*)diffuseData;
        //TexturesData& castedNormIn = *(TexturesData*)diffuseData;

        TexturesData& castedDiff = textures[0];
        TexturesData& castedNorm = textures[1];

        const auto& gldIn = TO_GL(diffuseData);
        const auto& glnIn = TO_GL(normalData);

        auto& gld = TO_GL(castedDiff);
        auto& gln = TO_GL(castedNorm);

        gld.id = gldIn.id;
        gld.offset = gldIn.offset;
        gld.size = gldIn.size;

        gln.id = glnIn.id;
        gln.offset = glnIn.offset;
        gln.size = glnIn.size;
    }

    void LImage::bindDiffuse(const char* path)
    {
        turnOnTexture();
        //imageResourceName = path;
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

    void LImage::init(ImageResource resource)
    {
        if(resource.name.size())
            textures = LResourceManager::loadImageResource(resource);
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

    TexturesData::~TexturesData()
    {
        if (textures)
        {
            //if (app->info.api == L_OPENGL)
                delete (OGLImageData*)textures;
            //else if (app->info.api == L_VULKAN)
            //    delete (VulkanImageData*)textures;
        }
    }
}