#pragma once

#include "LObject.h"
#include "vulkan/vulkan.h"
#include "include/GLEW/glew.h"
#include "enums.h"

namespace LGraphics
{
    class LApp;
    class LResourceManager;
    /*!
    @brief Класс изображения, привязывается к виджетом в качестве текстуры.
    */
    class LImage : public LObject
    {
    public:
        
        friend LApp;
        const char* getObjectType() const override { return "LImage"; }
        //auto getTexture() const { return texture; } ///< Возвращает текстуру.

#ifdef MEGATEXTURE_LG
        virtual void setDiffuse(const char* path);
#else
        void setDiffuse(GLuint id);
        void setDiffuse(VkImageView view);
#endif 

        const void* getTexture() const { return textures; }
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


        std::string getTexturePath() const { return texturePath; }
        void init();
        ~LImage();

    protected:

        //bool imageType;
        /*!
        @brief Конструктор.
        @param path - путь к изображению.
        */
        LImage(const char* path, RenderingAPI api);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        //LImage(const unsigned char* bytes, size_t size);
        void* textures = nullptr;

        //VkImageView texture = VkImageView();
        size_t mipLevels = 1;

        bool turnedOn = true; ///< Включена ли текстура.

        ::std::string texturePath;
        unsigned char* texturesBytes = nullptr;
        size_t texturesBytesSize = 0;
        RenderingAPI texturesType;

        static LResourceManager resManager;
    };
}

