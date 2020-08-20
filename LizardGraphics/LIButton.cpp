#include "LIButton.h"
#include "pch.h"

namespace LGraphics
{
    LIButton::LIButton(LApp* app, const char* path)
        :LRectangleShape(app, path)
    {
        setShader(new LShaders::Shader{ LShaders::interface_v, LShaders::interface_f });
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