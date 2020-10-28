#pragma once

#include "LRectangleBuffer.h"

namespace LGraphics
{
    class LTextBuffer : public LRectangleBuffer
    {
        //friend LSymbol;
    public:

        const char* getObjectType() override { return "LTextBuffer"; }
        LTextBuffer();
    protected:
        GLfloat** textCoords;
        const size_t charIndex_from = 32;
    };
}

