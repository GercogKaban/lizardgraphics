#include "LRectangleBuffer.h"
#include "LLogger.h"
#include "pch.h"

namespace LGraphics
{
    const std::vector<Vertex> LRectangleBuffer::vert =
    {
        {glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),glm::vec2(0.0,0.0) },
        {glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),glm::vec2(0.0,0.0) },
        {glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),glm::vec2(0.0,0.0) },
        {glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),glm::vec2(0.0,0.0) },
    };

    const std::vector<uint32_t> LRectangleBuffer::ind =
    {
        0,1,3,1,2,3
    };

    LRectangleBuffer::LRectangleBuffer(LApp* app)
        :LBuffer(app,vert,ind)
    {
    }
}