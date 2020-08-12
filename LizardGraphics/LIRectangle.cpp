#include "LApp.h"
#include "LIRectangle.h"
#include "LBaseComponent.h"
#include "pch.h"

namespace LGraphics
{
    LIRectangle::LIRectangle(LApp* app, LObject* parent, const char * path, LBaseComponent* component)
        :LRectangleShape(app, parent, path, component)
    {
        setShader(component->getShader());
    }

    void LIRectangle::tick()
    {
        if (app->getActiveWidget() == this)
            doAnimation();
    }
}