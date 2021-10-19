#pragma once

#include <vector>

#include "LObject.h"
#include "vulkan/vulkan.h"
#include "VulkanMemory/Allocator/include/vk_mem_alloc.h"
#include "include/GLEW/glew.h"
#include "enums.h"

#define NO_TEXTURE _UI32_MAX
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
        };

        struct ImageSkyboxResource
        {
            std::string name;
            std::string extension = ".png";
        };

        friend LApp;
        const char* getObjectType() const override { return "LImage"; }
        //auto getTexture() const { return texture; } ///< Возвращает текстуру.

#ifdef MEGATEXTURE_LG
        void setDiffuse(const TexturesData& data);
        void setNormal(const TexturesData& data);
        void setDisplacement(const TexturesData& data);
#else
        void setDiffuse(GLuint id);
        void setDiffuse(VkImageView view);
#endif 

        const auto& getTextures() const { return textures; }
        const auto& getDiffuse() const { return textures[0]; }
        const auto& getNormal() const { return textures[1]; }
        const auto& getParallax() const { return textures[2]; }
        /*!
        @brief Привязывает текстуру.
        @param path - путь к изображению.
        */
        void bindDiffuse(const char* path);

        /*!
        @brief Привязывает текстуру.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        //void bindTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel = 4);
        size_t getMipLevels() const { return mipLevels; }

        /*!
        @brief Выключает текстуру.
        */
        void turnOffTexture();

        /*!
        @brief Включает текстуру.
        */
        void turnOnTexture();

        /*!
        @brief Включает/выключает текстуру.
        @param show - true - включить текстуру, false - выключить.
        */
        void switchTexture(bool show);

        /*!
        @brief Включает/выключает текстуру.

        Если была включена, то текстура выключается,
        если была выключена, то текстура включается.
        */
        void switchTexture();

        /*!
        @brief Включена ли текстура
        */
        bool isTextureTurnedOn() const;

        std::string getNormalsPath() const { return normalsPath + imageResourceName; }
        std::string getDiffusePath() const { return diffusePath + imageResourceName; }
        std::string getDisplacementPath() const { return displacementPath + imageResourceName; }
        const std::string& getImageResourceName() const { return imageResourceName; }
        void init(ImageResource resource);
        ~LImage();

    protected:

        //bool imageType;
        /*!
        @brief Конструктор.
        @param path - путь к изображению.
        */
        LImage(ImageResource resource, RenderingAPI api);
        LImage(const TexturesData& diffuseData, const TexturesData& normalData, const TexturesData& displacementData);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        //LImage(const unsigned char* bytes, size_t size);
        std::vector<TexturesData> textures;

        //VkImageView texture = VkImageView();
        size_t mipLevels = 1;

        bool turnedOn = true; ///< Включена ли текстура.

        ::std::string displacementPath;
        ::std::string diffusePath;
        ::std::string normalsPath;
        ::std::string imageResourceName;
        RenderingAPI texturesType;

        static LResourceManager resManager;
    };
}

