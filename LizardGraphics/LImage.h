#pragma once

#include <vector>

#include "LObject.h"
#include "vulkan/vulkan.h"
#include "VulkanMemory/Allocator/include/vk_mem_alloc.h"
#include "include/GLEW/glew.h"
#include "constants.h"
#include "enums.h"

#define TO_GL(TEXTURES_DATA) *((TexturesData::OGLImageData*)TEXTURES_DATA.textures)

namespace LGraphics
{
    struct TexturesData;
    //using OpenGLTexture = TexturesData::OGLImageData::OGLTextureData;

    struct TexturesData
    {
        struct VulkanImageData
        {
            struct VulkanTextureData
            {
                VkImageView imageView = (VkImageView)nullptr;
                VkImage image;
                VmaAllocation allocData;
            };
            VulkanTextureData texture;
        };

        struct OGLImageData
        {
            GLuint id = NO_TEXTURE;
            glm::vec2 offset;
            glm::vec2 size;        
        };

        void* textures = nullptr;

        TexturesData(void* t)
        {
            textures = t;
        }

        TexturesData() {}

        TexturesData(TexturesData&& d) noexcept
            : textures(std::move(d.textures))
        {
            d.textures = nullptr;
        }

        TexturesData(const TexturesData& d)
        {
            textures = new TexturesData::OGLImageData;
            auto& gld = *(TexturesData::OGLImageData*)textures;
            const TexturesData::OGLImageData& gldIn = TO_GL(d);
            //auto& gld = TO_GL(casted);

            gld.id = gldIn.id;
            gld.offset = gldIn.offset;
            gld.size = gldIn.size;
        }


        ~TexturesData();
    };


    using OpenGLImage = TexturesData::OGLImageData;
    using VulkanTexture = TexturesData::VulkanImageData::VulkanTextureData;
    using VulkanImage = TexturesData::VulkanImageData;

    class LApp;
    class LResourceManager;
    class LModel;
    /*!
    @brief Класс изображения, привязывается к виджетом в качестве текстуры.
    */
    class LImage : public LObject
    {
    public:
        
        friend LResourceManager;
        friend LModel;
        struct ImageResource
        {
            std::string name;
            bool diffuse = true;
            bool normals = true;
            bool parallax = true;
            bool reflex = true;
        };

        struct ImageSkyboxResource
        {
            std::string name;
            std::string extension = ".png";
        };

        friend LApp;
        const char* getObjectType() const override { return "LImage"; }

        void setDiffuse(const TexturesData& data);
        void setNormal(const TexturesData& data);
        void setDisplacement(const TexturesData& data);
        void setReflex(const TexturesData& data);

        const auto& getTextures() const { return textures; }
        const auto& getDiffuse() const { return textures[0]; }
        const auto& getNormal() const { return textures[1]; }
        const auto& getParallax() const { return textures[2]; }
        const auto& getReflex() const { return textures[3]; }

        virtual void setNormalMapping(bool normalMapping) { this->normalMapping = normalMapping; }
        virtual void setParallaxMapping(bool parallaxMapping) { this->parallaxMapping = parallaxMapping; }
        virtual void setReflexMapping(bool reflexMapping) { this->reflexMapping = reflexMapping; }

        bool getNormalMapping() const { return normalMapping; }
        bool getParallaxMapping() const { return parallaxMapping; }
        bool getReflexMapping() const { return reflexMapping; }

        std::string getNormalsPath() const { return normalsPath + imageResourceName; }
        std::string getDiffusePath() const { return diffusePath + imageResourceName; }
        std::string getDisplacementPath() const { return displacementPath + imageResourceName; }
        std::string getReflexPath() const { return reflexPath + imageResourceName; }
        const std::string& getImageResourceName() const { return imageResourceName; }
        void init(ImageResource resource);
        ~LImage();

    protected:

        LImage(ImageResource resource, RenderingAPI api);
        LImage(const TexturesData& diffuseData, const TexturesData& normalData, const TexturesData& displacementData);

        std::vector<TexturesData> textures;

        size_t mipLevels = 1;

        bool turnedOn = true; ///< Включена ли текстура.

        ::std::string displacementPath;
        ::std::string diffusePath;
        ::std::string normalsPath;
        ::std::string reflexPath;
        ::std::string imageResourceName;
        RenderingAPI texturesType;

        static LResourceManager resManager;
        bool normalMapping;
        bool parallaxMapping;
        bool reflexMapping;
    };
}

