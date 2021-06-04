#pragma once

#include "LObject.h"
//#ifdef OPENGL
#include "include/GLEW/glew.h"
#include "vulkan/vulkan.h"
#include "VulkanMemoryAllocator/src/vk_mem_alloc.h"
//#endif

//#include "LApp.h"

namespace LGraphics
{
    class LApp;
    /*!
    @brief Абстрактный класс буфера вершин.

    LBuffer хранит в себе массив вершин объекта, массив текстурных координат,
    массив индексов (в каком порядке будут соединяться вершины при отрисовке).
    */
    class LBuffer : public LObject
    {
    public:
      
        const char* getObjectType() const override { return "LBuffer"; }
        //const GLuint* getVBO() const { return VBO; }

        /*!
        @brief Возвращает массив вершин.
        */
        auto getVertices() const { return vertices; }

        auto getIndices() const { return ebo; }

        /*!
        @brief Возвращает размер массива вершин.
        */
        virtual size_t getVertSize() const { return verticesCount * (coordsCount + textureCoordsCount + normalsCount) * sizeof(decltype(*vertices)); }

        ///*!
        //@brief Возвращает размер массива текстурных координат.
        //*/
        //size_t getTextureSize() const { return textureCoordsCount * verticesCount * sizeof(GLfloat); }

        /*!
        @brief Возвращает размер массива индексов.
        */
        virtual size_t getIndSize() const { return indicesCount * sizeof(decltype(*ebo)); }

        /*!
        @brief Возвращает количество индексов.
        */
        size_t getIndCount() const { return indicesCount; }

        /*!
        @brief Возвращает количество вершин.
        */
        size_t getVerticesCount() const { return verticesCount; }

        const GLuint getVaoNum() const { return VAO; }

        void init(LApp* app);
        VkBuffer& getVertBuffer() { return vertexBuffer; }
        VkBuffer& getIndBuffer() { return indexBuffer; }

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
        virtual void genBuffers();

        float* textures = nullptr; ///< Массив текстурных координат.

#ifdef VULKAN
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferMemory = VK_NULL_HANDLE;
        VmaAllocation indexBufferMemory = VK_NULL_HANDLE;
        LApp* app;
#endif // VULKAN

//#ifdef OPENGL
        float* vertices = nullptr; ///< Массив вершин.*/
        uint16_t* ebo = nullptr;       ///< Массив индексов.*/
//#endif
        uint32_t VBO,VAO,EBO;
        //uint32_t VBO[4]/*на всякий случай больше, для потомков*/, VAO, EBO;     ///< OpenGL буфферы.*/

        const size_t coordsCount = 3;
        size_t verticesCount, indicesCount, normalsCount, textureCoordsCount = 0; ///< Кол-во вершин, индексов, координат, текстурных координат.*/
    };
}

