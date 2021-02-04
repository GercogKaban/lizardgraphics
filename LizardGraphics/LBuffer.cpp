#include "LBuffer.h"
#include "pch.h"

namespace LGraphics
{
#ifdef OPENGL
    void LBuffer::init()
    {
        setBuffers();
    }
#endif

#ifdef VULKAN
    void LBuffer::init(LApp* app)
    {
        this->app = app;
        setBuffers();
    }
#endif

    LBuffer::LBuffer()
    {
    }

    void LBuffer::genBuffers()
    {
#ifdef OPENGL
        glGenVertexArrays(1, &VAO);
        glGenBuffers(2, VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndSize(), ebo, GL_STATIC_DRAW);

        glVertexAttribPointer(0, coordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, getTextureSize(), textures, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(1, textureCoordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
#endif OPENGL

#ifdef VULKAN
        app->createBuffer(vertices, vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getVertSize());
        app->createBuffer(ebo, indexBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getIndSize());
#endif
    }

    LBuffer::~LBuffer()
    {
#ifdef OPENGL
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(2, VBO);
        glDeleteVertexArrays(1, &VAO);
#endif OPENGL
#ifdef VULKAN
        if (vertexBuffer)
        {
            vkDestroyBuffer(app->g_Device, vertexBuffer, nullptr);
            vkFreeMemory(app->g_Device, vertexBufferMemory, nullptr);
        }

        if (indexBuffer)
        {
            vkDestroyBuffer(app->g_Device, indexBuffer, nullptr);
            vkFreeMemory(app->g_Device, indexBufferMemory, nullptr);
        }
#endif
        if (vertices) delete[] vertices;
        if (ebo) delete[] ebo;
        //delete[] textures;
    }
}