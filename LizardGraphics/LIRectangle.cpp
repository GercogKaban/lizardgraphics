#include "LApp.h"
#include "LIRectangle.h"
#include "pch.h"

namespace LGraphics
{
    LIRectangle::LIRectangle(LApp* app, const char * path)
        :LRectangleShape(app, path){}

    void LIRectangle::tick()
    {
        if (app->getActiveWidget() == this)
            doAnimation();
    }
}