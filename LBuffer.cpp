#include "LBuffer.h"
#include "pch.h"

namespace LGraphics
{
    void LBuffer::init()
    {
        setBuffers();
    }

    LBuffer::LBuffer()
    {
    }

    void LBuffer::genBuffers()
    {
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
    }

    LBuffer::~LBuffer()
    {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(2, VBO);
        glDeleteVertexArrays(1, &VAO);
        delete[] vertices;
        delete[] ebo;
        delete[] textures;
    }
}