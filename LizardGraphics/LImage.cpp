#include "pch.h"
#include "LImage.h"
#include "LLogger.h"
#include "LApp.h"

#include "LResourceManager.h"

namespace LGraphics
{
#ifdef MEGATEXTURE_LG

    void LImage::setDiffuse(const TexturesData& data)
    {
        auto& diff = TO_GL(textures[0]);
        const auto& in = TO_GL(data);
        diff.id = in.id;
        diff.offset = in.offset;
        diff.size = in.size;
    }

    void LImage::setNormal(const TexturesData& data)
    {
        auto& norm = TO_GL(textures[1]);
        const auto& in = TO_GL(data);
        norm.id = in.id;
        norm.offset = in.offset;
        norm.size = in.size;
    }

    void LImage::setDisplacement(const TexturesData& data)
    {
        auto& parallax = TO_GL(textures[2]);
        const auto& in = TO_GL(data);
        parallax.id = in.id;
        parallax.offset = in.offset;
        parallax.size = in.size;
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

    LImage::LImage(const TexturesData& diffuseData, const TexturesData& normalData, const TexturesData& displacementData)
    {
        textures.push_back (TexturesData(new TexturesData::OGLImageData));
        textures.push_back(TexturesData(new TexturesData::OGLImageData));
        textures.push_back(TexturesData(new TexturesData::OGLImageData));

        setDiffuse(diffuseData);
        setNormal(normalData);
        setDisplacement(displacementData);
    }

    LImage::LImage(ImageResource resource, RenderingAPI api)
    {
        texturesType = api;
        imageResourceName = resource.name;
        init(resource);
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
        //if (textures)
        //{
        //    delete (OGLImageData*)textures;
        //    textures = nullptr;
        //}
    }
}