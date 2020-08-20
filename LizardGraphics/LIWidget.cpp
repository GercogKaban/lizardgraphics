#include "LIWidget.h"
#include "pch.h"

void LGraphics::LIWidget::setAnimation(std::function<void()> fun)
{
    animation = fun;
}

void LGraphics::LIWidget::doAnimation()
{
    animation();
}

void LGraphics::LIWidget::setBreakingAnimation(std::function<void()> fun)
{
    breakingAnimation = fun;
}

void LGraphics::LIWidget::breakAnimation()
{
    breakingAnimation();
}
