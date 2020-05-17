#pragma once
#include "LShape.h"

namespace LGraphics
{
    class LApp;
    class LBuffer;
    class LBaseComponent;

    class LRectangleShape : public LShape
    {
    public:

        //std::list<LWidgetI*> intersectsWith() override;
        bool mouseOnIt() override;

        void draw() override;
        LRectangleShape(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* = nullptr);
        LRectangleShape(LApp* app, LObject* parent = nullptr, const unsigned char* bytes = nullptr, size_t size = 0, LBaseComponent* = nullptr);
    };
}

