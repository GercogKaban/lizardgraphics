#pragma once
#include "Lshaders.h"
#include "LRectangleBuffer.h"

namespace LGraphics
{
    struct LBaseComponent
    {
        LShaders::Shader *interfaceShader;
        LBuffer* baseRectangleBuffer;

        LBaseComponent()
        {
            interfaceShader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
            baseRectangleBuffer = new LRectangleBuffer();
            baseRectangleBuffer->init();
        }

        ~LBaseComponent()
        {
            delete interfaceShader;
            delete baseRectangleBuffer;
        }
    };
}
