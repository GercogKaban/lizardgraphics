#pragma once

#include "LApp.h"

namespace LGraphics
{
    class LBaseComponent;
    class LIButton : public LRectangleShape
    {
    public:

        friend LApp;

        LIButton(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* = nullptr);

        void setClickEventFunction(std::function<void()> fun);


    protected:

        std::function<void()> clickFunction;
        void doClickEventFunction();
    };
}

