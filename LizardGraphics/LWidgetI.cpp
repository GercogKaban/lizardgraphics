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

void LGraphics::LWidgetI::setBreakingAnimation(std::function<void()> fun)
{
    breakingAnimation = fun;
}

void LGraphics::LWidgetI::breakAnimation()
{
    breakingAnimation();
}
