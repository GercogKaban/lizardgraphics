#pragma once
#include "LRectangleShape.h"

namespace LGraphics
{
    class LBaseComponent;
    class LIRectangle : public LRectangleShape
    {
    public:
        LIRectangle(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* = nullptr);
    };
}

