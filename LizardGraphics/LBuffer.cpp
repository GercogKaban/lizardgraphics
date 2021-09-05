#include "LBuffer.h"
#include "LLogger.h"
#include "LApp.h"
#include "pch.h"

namespace LGraphics
{
    void LBuffer::init(LApp* app)
    {
        LOG_CALL
        this->app = app;
        setBuffers();
    }

    LBuffer::LBuffer()
    {
    }

    void LBuffer::genBuffers()
    {
        LOG_CALL
        if (app->info.api == L_OPENGL)
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndSize(), ebo, GL_STATIC_DRAW);

            glVertexAttribPointer(0, coordsCount, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, normalsCount, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, textureCoordsCount, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
        }

        //else if (app->info.api == L_VULKAN)
        //{
        //    app->createBuffer(vertices, vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getVertSize());
        //    app->createBuffer(ebo, indexBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getIndSize());
        //}
    }

    LBuffer::~LBuffer()
    {
        if (app->info.api == L_OPENGL)
        {
            glDeleteBuffers(1, &EBO);
            glDeleteBuffers(1, &VBO);
            glDeleteVertexArrays(1, &VAO);
        }
        //else if (app->info.api == L_VULKAN)
        //{
        //    if (vertexBuffer)
        //        vmaDestroyBuffer(app->allocator, vertexBuffer, vertexBufferMemory);

        //    if (indexBuffer)
        //        vmaDestroyBuffer(app->allocator, indexBuffer, indexBufferMemory);
        //}
        if (vertices) delete[] vertices;
        if (ebo) delete[] ebo;
    }
}