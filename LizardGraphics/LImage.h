#pragma once

#include "LObject.h"

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

#ifdef OPENGL
        void setTexture(GLuint id) { texture = id; }
#endif
#ifdef VULKAN
        void setTexture(VkImageView view) { texture = view; }
#endif
        /*!
        @brief Привязывает текстуру.
        @param path - путь к изображению.
        */
        void bindTexture(const char* path, int desiredChannel = 4);

#ifdef VULKAN
        /*!
        @brief Привязывает текстуру.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        void bindTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel = 4);
        size_t getMipLevels() const { return mipLevels; }
#endif


#ifdef VULKAN
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
#endif
        ~LImage();

    protected:

        /*!
        @brief Конструктор.
        @param path - путь к изображению.
        */
        LImage(const char* path);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        LImage(const unsigned char* bytes, size_t size);
#ifdef OPENGL
        GLuint texture; ///< Дескриптор текстуры.
#endif

#ifdef VULKAN
        VkImageView texture = VkImageView();
        size_t mipLevels = 1;
#endif

        bool turnedOn = true; ///< Включена ли текстура.

        std::string texturePath;
        unsigned char* texturesBytes = nullptr;
        size_t texturesBytesSize = 0;

        static LResourceManager resManager;
    };
}

