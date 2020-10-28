#pragma once

#include "LRectangleBuffer.h"

namespace LGraphics
{
    class LTextBuffer : public LRectangleBuffer
    {
        //friend LSymbol;
    public:
        LTextBuffer();
    protected:
        GLfloat** textCoords;
        const size_t charIndex_from = 32;
    };
}

