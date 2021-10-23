#pragma once

#include "constants.h"
#include "LObject.h"
//#ifdef OPENGL
#include "include/GLEW/glew.h"
#include "vulkan/vulkan.h"
//#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"
//#endif

//#include "LApp.h"

namespace LGraphics
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        int BoneIDs[MAX_BONE_INFLUENCE];
        float BoneWeights[MAX_BONE_INFLUENCE];
    };

    class LApp;
    /*!
    @brief Абстрактный класс буфера вершин.

    LBuffer хранит в себе массив вершин объекта, массив текстурных координат,
    массив индексов
    */
    class LBuffer : public LObject
    {
    public:
      
        const char* getObjectType() const override { return "LBuffer"; }
        //const GLuint* getVBO() const { return VBO; }

        /*!
        @brief Возвращает массив вершин.
        */
        const auto& getVertices() const { return vertices; }

        const auto& getIndices() const { return ebo; }

        /*!
        @brief Возвращает размер массива индексов.
        */
        virtual size_t getIndSize() const { return ebo.size() * sizeof(decltype(*ebo.data())); }
        virtual size_t getVertSize() const { return vertices.size() * sizeof(Vertex); }

        const GLuint getVaoNum() const { return VAO; }

        //VkBuffer& getVertBuffer() { return vertexBuffer; }
        //VkBuffer& getIndBuffer() { return indexBuffer; }

        /*!
        @brief Деструктор LBuffer.
        */
        virtual ~LBuffer();

        LBuffer(LApp* app, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        void setGeometry(const std::vector<Vertex>& vertices);
        void setGeometry(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    protected:

        virtual void genBuffers();

#ifdef VULKAN
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferMemory = VK_NULL_HANDLE;
        VmaAllocation indexBufferMemory = VK_NULL_HANDLE;
#endif // VULKAN

        LApp* app;
        std::vector<Vertex> vertices; ///< Массив вершин.*/
        std::vector<uint32_t> ebo;       ///< Массив индексов.*/
        GLuint VBO,VAO,EBO;
    };
}

