#include "pch.h"

#include "LMultiWRectangle.h"
#include "LMultiRectangleBuffer.h"
#include "LLogger.h"
//#include "LApp.h"
#include "additional.h"

//#include <iostream>

//#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LMultiWRectangle::LMultiWRectangle(LApp* app, size_t rectCount)
    : LWidgetGroup<LWRectangle>(app), rectCount(rectCount)
{
    LOG_CALL
    //shader = app->multi_defaultShader;
    view = app->getViewMatrix();
    projection = app->getProjectionMatrix();

    buffer = new LMultiRectangleBuffer(rectCount);
}

//glm::vec2 LGraphics::LWRectangle::getScreenCoords() const
//{
//    glm::vec4 coords_ = glm::vec4{
//    (((LRectangleBuffer*)buffer)->getBottomLeftCorner().x + ((LRectangleBuffer*)buffer)->getBottomRightCorner().x)/2.0f,
//    (((LRectangleBuffer*)buffer)->getTopRightCorner().y + ((LRectangleBuffer*)buffer)->getBottomRightCorner().y) / 2.0f,
//    0.0f, 1.0f};
//    return projection * app->getViewMatrix() * calculateModelMatrix() * coords_;
//}

//glm::mat4 LGraphics::LWRectangle::calculateModelMatrix() const
//{
//    glm::mat4 model_ = glm::mat4(1.0f);
//    model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
//    model_ *= rotate_;
//    model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
//    return model_;
//}
