#include "LBuffer.h"
#include "LLogger.h"
#include "LApp.h"
#include "pch.h"

namespace LGraphics
{
    LBuffer::LBuffer(LApp* app, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        LOG_CALL
        this->app = app;
        this->vertices = std::vector<Vertex>(std::move(vertices));
        this->ebo = std::vector<uint32_t>(std::move(indices));
        genBuffers();
    }

    void LBuffer::setGeometry(const std::vector<Vertex>& vertices)
    {
        this->vertices = vertices;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
    }

    void LBuffer::setGeometry(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        setGeometry(vertices);
        this->ebo = indices;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo.size() * sizeof(decltype(*ebo.data())), ebo.data(), GL_STATIC_DRAW);
    }

    
    void LBuffer::genBuffers()
    {
        LOG_CALL
        if (app->info.api == L_OPENGL)
        {
            for (const auto& v : vertices)
                for (size_t i = 0; i < MAX_BONE_INFLUENCE; ++i)
                    if (v.BoneIDs[i] >= MAX_BONES && v.BoneIDs[i] != -1 || v.BoneIDs[i] <= -2)
                        throw std::runtime_error("bone id is incorrect!");

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            if (ebo.size())
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo.size() * sizeof(decltype(*ebo.data())), ebo.data(), GL_STATIC_DRAW);
            }

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,TexCoords));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,Tangent));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
            glEnableVertexAttribArray(4);

            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BoneIDs));
            glEnableVertexAttribArray(5);

            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BoneWeights));
            glEnableVertexAttribArray(6);

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
    }
}