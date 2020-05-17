#pragma once

#include "LBuffer.h"

namespace LGraphics
{
    class LRectangleBuffer : public LBuffer
    {
    protected:
        void setBuffers() override;
        void setVerts() override;
        void setInds() override;
    };

    //namespace
    //{
    //    LBuffer* baseRectangleBuffer;
    //}
}

