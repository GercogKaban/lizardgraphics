#include "pch.h"
#include "LPlane.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LPlane::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LPlane::objChanged;
bool LGraphics::LPlane::needToResetBuffer = false;
GLuint LGraphics::LPlane::vbo;

LGraphics::LPlane::LPlane(LApp* app, ImageResource res)
    :LImagedShape(app, res,objChanged,uniforms,needToResetBuffer)
{
    LOG_CALL
    setBuffer(app->plane->getMehes()[0].buffer);
}

void LGraphics::LPlane::drawInstanced()
{
    LImagedShape::drawInstanced(L_PLANE, objChanged, needToResetBuffer, app->plane->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->plane->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LPlane::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LPlane::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LPlane::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LPlane::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LPlane::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LPlane::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LPlane::setNormalMapping(bool normalMapping)
{
    LImagedShape::setNormalMapping(objChanged, this, normalMapping);
}

void LGraphics::LPlane::setParallaxMapping(bool parallaxMapping)
{
    LImagedShape::setParallaxMapping(objChanged, this, parallaxMapping);
}

void LGraphics::LPlane::updateBuffer()
{
    LImagedShape::updateBuffer(L_PLANE,objChanged, needToResetBuffer, app->plane->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LPlane::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LPlane::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->plane->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

//glm::mat4 LGraphics::LPlane::calculateModelMatrix() const
//{
//#if _DEBUG
//#include "../Optimized/optimized.h"
//    return _calculateModelMatrix(move_, rotate_, scale_);
//#else
//    glm::mat4 model_ = glm::mat4(1.0f);
//    model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
//    model_ *= rotate_;
//    model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
//    return model_;
//#endif
//}
