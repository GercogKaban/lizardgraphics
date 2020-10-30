#include "LApp.h"
#include "LIButton.h"
#include "pch.h"

namespace LGraphics
{
    LIButton::LIButton(LApp* app, const char* path)
        :LRectangleShape(app, path){}

    LIButton::LIButton(LApp * app, const unsigned char * bytes, size_t bytesSize)
        : LRectangleShape(app,bytes,bytesSize){}

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