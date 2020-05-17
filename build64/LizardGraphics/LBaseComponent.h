#pragma once
#include "Lshaders.h"
#include "LRectangleBuffer.h"

namespace LGraphics
{
    class LBaseComponent
    {
        LShaders::Shader *interfaceShader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
        LRectangleBuffer* baseRectangleBuffer = new LRectangleBuffer();
    public:

        ~LBaseComponent()
        {
            delete interfaceShader;
            delete baseRectangleBuffer;
        }
    };

}
