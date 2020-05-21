#include "LIRectangle.h"
#include "LBaseComponent.h"
#include "pch.h"

namespace LGraphics
{
    LIRectangle::LIRectangle(LApp* app, LObject* parent, const char * path, LBaseComponent* component)
        :LRectangleShape(app, parent, path, component)
    {
        setShader(component->interfaceShader);
    }
}