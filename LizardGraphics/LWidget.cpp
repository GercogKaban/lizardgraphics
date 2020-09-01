#include "LWidget.h"
#include "LApp.h"
#include "pch.h"

void LGraphics::LWidget::setAnimation(std::function<void()> fun)
{
    animation = fun;
}

void LGraphics::LWidget::doAnimation()
{
    animation();
}

void LGraphics::LWidget::setBreakingAnimation(std::function<void()> fun)
{
    breakingAnimation = fun;
}

void LGraphics::LWidget::breakAnimation()
{
    breakingAnimation();
}

LGraphics::LWidget::LWidget(LApp* app, const char * path, bool lazy)
    :LImage(path, lazy)
{
    init(app, lazy);
}

LGraphics::LWidget::LWidget(LApp* app, const unsigned char * bytes, size_t size, bool lazy)
    : LImage(bytes, size, lazy)
{
    init(app, lazy);
}

void LGraphics::LWidget::init(LApp* app, bool lazy)
{
    this->app = app;
    if (lazy) app->addTextureToInit(this);
}
