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

LGraphics::LWidget::LWidget(LApp* app, const char * path)
    :LImage(path)
{
    this->app = app;
}

LGraphics::LWidget::LWidget(LApp* app, const unsigned char * bytes, size_t size)
    : LImage(bytes, size)
{
    this->app = app;
}

void LGraphics::LWidget::init(LApp* app)
{
    this->app = app;
}

void LGraphics::LWidget::init()
{
    this->LImage::init();
    isInited_ = true;
}

void LGraphics::LWidget::drawOver(bool over)
{
    drawOver_ = over;
    for (auto& w : innerWidgets)
        w->drawOver(over);
}

LGraphics::LWidget* LGraphics::LWidget::getWidgetByName(const char * name)
{
    for (auto& w : *app->getObjects())
        if (w->name == name)
            return w;
    return nullptr;
}

LGraphics::LWidget * LGraphics::LWidget::getInnerWidgetByName(const char * name)
{
    for (auto& w : innerWidgets)
        if (w->name == name)
            return w;
    return nullptr;
}

LGraphics::LWidget::~LWidget()
{
    // нужна рекурсия
    //app->deleteWidget(this);
}
