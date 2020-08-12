#include "LIButton.h"
#include "LBaseComponent.h"
#include "pch.h"

namespace LGraphics
{
    LIButton::LIButton(LApp* app, LObject* parent, const char* path, LBaseComponent* buffer)
        :LRectangleShape(app, parent, path, buffer)
    {
        setShader(buffer->getShader());
    }

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