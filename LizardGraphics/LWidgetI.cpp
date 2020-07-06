#include "LWidgetI.h"
#include "pch.h"

void LGraphics::LWidgetI::setAnimation(std::function<void()> fun)
{
    animation = fun;
}

void LGraphics::LWidgetI::doAnimation()
{
    animation();
}
