#pragma once
#include "LRectangleShape.h"

namespace LGraphics
{
    class LModel : public LRectangleShape
    {
    public:

        LModel(LApp* app, const char* path);

        void loadModel(const char* path);
    };
}