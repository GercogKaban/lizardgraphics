#include "LApp.h"
#include "LIRectangle.h"
#include "pch.h"

namespace LGraphics
{
    LIRectangle::LIRectangle(LApp* app, const char * path)
        :LRectangleShape(app, path)
    {
        setShader(new LShaders::Shader{ LShaders::interface_v, LShaders::interface_f });
    }

    void LIRectangle::tick()
    {
        if (app->getActiveWidget() == this)
            doAnimation();
    }
}