#include "LApp.h"
#include "LIButton.h"
#include "pch.h"

namespace LGraphics
{
    LIButton::LIButton(LApp* app, const char* path, bool lazy)
        :LRectangleShape(app, path, lazy){}

    void LIButton::setClickEventFunction(std::function<void()> fun)
    {
        clickFunction = fun;
    }

    void LIButton::tick()
    {
        if (app->getActiveWidget() == this)
            doAnimation();
    }

    void LIButton::doClickEventFunction()
    {
        clickFunction();
    }

 
}