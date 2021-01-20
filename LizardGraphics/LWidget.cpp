#include "LWidget.h"
#include "LApp.h"
#include "pch.h"

void LGraphics::LWidget::rotateX(float angleDegree)
{
    rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 1.0f,0.0f,0.0f });
    changed = app->wd->ImageCount;
}

void LGraphics::LWidget::rotateY(float angleDegree)
{
    rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 0.0f,1.0f,0.0f });
    changed = app->wd->ImageCount;
}

void LGraphics::LWidget::rotateZ(float angleDegree)
{
    rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 0.0f,0.0f,1.0f });
    changed = app->wd->ImageCount;
}

void LGraphics::LWidget::setRotate(const glm::mat4 & rotate)
{
    rotate_ = rotate; 
    changed = app->wd->ImageCount;
}

//void LGraphics::LWidget::setRotate(fvect3 val)
//{
//    rotateX_ = val.x;
//    rotateY_ = val.y;
//    rotateZ_ = val.z;
//}

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
    changed = app->wd->ImageCount;
}

#ifdef OPENGL
LGraphics::LWidget::LWidget(LApp* app, const unsigned char * bytes, size_t size)
    : LImage(bytes, size)
{
    this->app = app;
    changed = app->wd->ImageCount;
}
#endif

//void LGraphics::LWidget::init(LApp* app)
//{
//    this->app = app;
//}

void LGraphics::LWidget::init()
{
    this->LImage::init();
    isInited_ = true;
}

//LGraphics::LWidget* LGraphics::LWidget::getWidgetByName(const char * name)
//{
//    for (auto& w : *app->getObjects())
//        if (w->name == name)
//            return w;
//    return nullptr;
//}

LGraphics::LWidget * LGraphics::LWidget::getInnerWidgetByName(const char * name)
{
    if (innerWidgets)
    for (auto& w : *innerWidgets)
        if (w->name == name)
            return w;
    return nullptr;
}

void LGraphics::LWidget::addInnerWidget(LWidget* innerWidget)
{
    if (!innerWidgets) innerWidgets = new std::vector<LGraphics::LWidget*>();

    innerWidgets->push_back(innerWidget);
    std::vector<LWidget*>& obj = app->getNonInterfaceObjects();
    for (size_t i = 0; i < obj.size(); ++i)
        if (obj[i] == innerWidget)
            obj.erase(obj.begin() + i);
}

LGraphics::LWidget::~LWidget()
{
    //app->deleteWidget(this);
}
