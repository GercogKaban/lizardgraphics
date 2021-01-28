#pragma once

#include "LBuffer.h"

namespace LGraphics
{
    class LModelBuffer : public LBuffer
    {
    public:

        void setBuffers() override;

        LModelBuffer(LApp* app, float* vertices, uint16_t* indices, size_t verticesCount, size_t indicesCount,
        size_t normalsCount);
    };
}