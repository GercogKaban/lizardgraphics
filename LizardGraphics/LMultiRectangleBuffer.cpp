﻿#include "LMultiRectangleBuffer.h"
#include "LLogger.h"
#include "pch.h"

namespace LGraphics
{
    void LMultiRectangleBuffer::setBuffers()
    {
        LOG_CALL
        verticesCount = 4; indicesCount = 6 * rectCount, textureCoordsCount = 2;

        setVerts();
        setInds();
        genBuffers();
    }

    void LMultiRectangleBuffer::genBuffers()
    {
        LOG_CALL
#ifdef OPENGL
        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndSize(), ebo, GL_STATIC_DRAW);

        glVertexAttribPointer(0, coordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, getTextureSize(), textures, GL_STATIC_DRAW);
        glVertexAttribPointer(1, textureCoordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, getIndSize(), objIndex, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
#endif OPENGL
    }


    void LMultiRectangleBuffer::setVerts()
    {
        LOG_CALL
        vertices = new float[coordsCount * verticesCount]
        {
             0.5f,  0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };

        textures = new float[textureCoordsCount* verticesCount]
        {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };
    }

    void LMultiRectangleBuffer::setInds()
    {
        LOG_CALL
        ebo = new uint16_t[indicesCount];

        for (size_t i = 0; i < rectCount; i++)
        {
            ebo[6 * i + 0] = 0;
            ebo[6 * i + 1] = 1;
            ebo[6 * i + 2] = 3;
            ebo[6 * i + 3] = 1;
            ebo[6 * i + 4] = 2;
            ebo[6 * i + 5] = 3;
        }
    }

    LMultiRectangleBuffer::LMultiRectangleBuffer(size_t rectCount) : rectCount(rectCount)
    {
        LOG_CALL
#ifdef OPENGL
        init();
#endif
#ifdef VULKAN
        init(app);
#endif
    }
}