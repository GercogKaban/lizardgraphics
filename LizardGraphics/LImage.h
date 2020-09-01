#pragma once

#include "LObject.h"
#include "LResourceManager.h"

namespace LGraphics
{
    /*!
    @brief Класс изображения, привязывается к виджетом в качестве текстуры.
    */
    class LImage : public LObject
    {
    public:
        
        GLuint getTexture() const { return texture; } ///< Возвращает текстуру.

        /*!
        @brief Привязывает текстуру.
        @param path - путь к изображению.
        */
        void bindTexture(const char* path);

        /*!
        @brief Привязывает текстуру.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        void bindTexture(const unsigned char* bytes, size_t size);

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

        void init();

        ~LImage();

    protected:

        /*!
        @brief Конструктор.
        @param path - путь к изображению.
        */
        LImage(const char* path, bool lazy = true);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        LImage(const unsigned char* bytes, size_t size, bool lazy = true);

        GLuint texture; ///< Дескриптор текстуры.

        bool turnedOn = true; ///< Включена ли текстура.

        std::string texturePath;
        unsigned char* texturesBytes = nullptr;
        size_t texturesBytesSize = 0;

        static LResourceManager resManager;
    };
}

