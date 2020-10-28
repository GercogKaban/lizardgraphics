#pragma once

#include "LObject.h"
#include "include/GLEW/glew.h"

namespace LGraphics
{
    /*!
    @brief Абстрактный класс буфера вершин.

    LBuffer хранит в себе массив вершин объекта, массив текстурных координат,
    массив индексов (в каком порядке будут соединяться вершины при отрисовке).
    */
    class LBuffer : public LObject
    {
    public:

        //const GLuint* getVBO() const { return VBO; }

        /*!
        @brief Возвращает объект вершинного массива.
        */
        const GLuint getVaoNum() const { return VAO; }

        /*!
        @brief Возвращает массив вершин.
        */
        const auto getVertices() const { return vertices; }

        /*!
        @brief Возвращает размер массива вершин.
        */
        size_t getVertSize() const { return verticesCount * coordsCount * sizeof(GLfloat); }

        /*!
        @brief Возвращает размер массива текстурных координат.
        */
        size_t getTextureSize() const { return textureCoordsCount * verticesCount * sizeof(GLfloat); }

        /*!
        @brief Возвращает размер массива индексов.
        */
        size_t getIndSize() const { return indicesCount * sizeof(GLuint); }

        /*!
        @brief Возвращает количество индексов.
        */
        size_t getIndCount() const { return indicesCount; }

        /*!
        @brief Возвращает количество вершин.
        */
        size_t getVerticesCount() const { return verticesCount; }

        /*!
        @brief Инициализирует LBuffer.
        */
        void init();

        /*!
        @brief Деструктор LBuffer.
        */
        virtual ~LBuffer();

    protected:

        LBuffer();

        /*!
        @brief Инициализирует все буфферы (вершины, индексы).
        */
        virtual void setBuffers() = 0;

        /*!
        @brief Инициализирует массив вершин.
        */
        virtual void setVerts() = 0;

        /*!
        @brief Инициализирует массив индексов.
        */
        virtual void setInds() = 0;

        /*!
        @brief Создаёт VAO и привязывает к нему VBO,EBO.
        */
        void genBuffers();

        GLfloat* textures = nullptr; ///< Массив текстурных координат.
        GLfloat* vertices = nullptr; ///< Массив вершин.*/
        GLuint* ebo = nullptr;       ///< Массив индексов.*/
        GLuint VBO[2], VAO, EBO;     ///< OpenGL буфферы.*/

        const size_t coordsCount = 3;
        size_t verticesCount, indicesCount, textureCoordsCount = 0; ///< Кол-во вершин, индексов, координат, текстурных координат.*/

    };
}

