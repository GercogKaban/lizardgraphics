#include "LRectangleBuffer.h"
#include "pch.h"

namespace LGraphics
{
    void LRectangleBuffer::setBuffers()
    {
        verticesCount = 4; indicesCount = 6, textureCoordsCount = 2;

        setVerts();
        setInds();
        genBuffers();
    }

    void LRectangleBuffer::setVerts()
    {
        vertices = new GLfloat[coordsCount * verticesCount]
        {
             0.5f,  0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };

        textures = new GLfloat[textureCoordsCount* verticesCount]
        {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };
    }

    void LRectangleBuffer::setInds()
    {
        ebo = new GLuint[indicesCount]
        {
            0,1,3,
            1,2,3
        };
    }
}