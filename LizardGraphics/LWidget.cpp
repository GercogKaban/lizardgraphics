#include "LWidget.h"
#include "LApp.h"
#include "pch.h"

LGraphics::LApp* LGraphics::LWidget::app = nullptr;

void LGraphics::LWidget::rotateX(float angleDegree)
{
    //rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 1.0f,0.0f,0.0f });
    rotate_q = glm::rotate(rotate_q, glm::radians(angleDegree), { 1.0f,0.0f,0.0f });

    rotateDegrees[0] += angleDegree;
    if (rotateDegrees[0] >= 360)
        rotateDegrees[0] -= 360;
    else if (rotateDegrees[0] <= -360)
        rotateDegrees[0] += 360;
    setUpdateUniformsFlag();
}

void LGraphics::LWidget::rotateY(float angleDegree)
{
    //rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 0.0f,1.0f,0.0f });
    rotate_q = glm::rotate(rotate_q, glm::radians(angleDegree), { 0.0f,1.0f,0.0f });

    rotateDegrees[1] += angleDegree;
    if (rotateDegrees[1] >= 360)
        rotateDegrees[1] -= 360;
    else if (rotateDegrees[1] <= -360)
        rotateDegrees[1] += 360;
    setUpdateUniformsFlag();
}

void LGraphics::LWidget::rotateZ(float angleDegree)
{
    //rotate_ = glm::rotate(rotate_, glm::radians(angleDegree), { 0.0f,0.0f,1.0f });
    rotate_q = glm::rotate(rotate_q, glm::radians(angleDegree), { 0.0f,0.0f,1.0f });

    rotateDegrees[2] += angleDegree;
    if (rotateDegrees[2] >= 360)
        rotateDegrees[2] -= 360;
    else if (rotateDegrees[2] <= -360)
        rotateDegrees[2] += 360;
    setUpdateUniformsFlag();
}

void LGraphics::LWidget::setRotate(const glm::mat4& rotate)
{
    //rotate_ = rotate; 
    rotate_q = glm::quat(rotate);
    setUpdateUniformsFlag();
}

LGraphics::LWidget::~LWidget()
{
}

void LGraphics::LWidget::setUpdateUniformsFlag()
{
    if (app->getAppInfo().api == L_VULKAN)
        changed = app->wd->ImageCount;
    else
        changed = 1;
}

LGraphics::LWidget::LWidget(LApp* app)
{
    this->app = app;
    auto ranges = app->lastLoadedRanges;
    rangeX = ranges[0]; rangeY = ranges[1]; rangeZ = ranges[2];
    setUpdateUniformsFlag();
}
