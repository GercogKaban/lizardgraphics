#include "LApp.h"
#include "LIRectangle.h"
#include "pch.h"

namespace LGraphics
{
    LIRectangle::LIRectangle(LApp* app, const char * path, bool lazy)
        :LRectangleShape(app, path,lazy){}

    void LIRectangle::tick()
    {
        if (app->getActiveWidget() == this)
            doAnimation();
    }
}