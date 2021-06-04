#include "LRectangleBuffer.h"
#include "LLogger.h"
#include "pch.h"

namespace LGraphics
{
    void LRectangleBuffer::setBuffers()
    {
        LOG_CALL
        verticesCount = 4; indicesCount = 6, textureCoordsCount = 2, normalsCount = 3;

        setVerts();
        setInds();
        genBuffers();
    }

    void LRectangleBuffer::setVerts()
    {
        LOG_CALL
        vertices = new float[/*(coordsCount + textureCoordsCount + normalsCount) * verticesCount*/]
        {
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        };
    }

    void LRectangleBuffer::setInds()
    {
        LOG_CALL
        ebo = new uint16_t[indicesCount]
        {
            0,1,3,
            1,2,3
        };
    }

    LRectangleBuffer::LRectangleBuffer(LApp* app)
    {
        LOG_CALL
        init(app);
    }
}