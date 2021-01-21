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

void LGraphics::LWidget::init()
{
    this->LImage::init();
    isInited_ = true;
}

