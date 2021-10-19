#include "LCylinder.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LCylinder::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LCylinder::objChanged;
bool LGraphics::LCylinder::needToResetBuffer = false;
GLuint LGraphics::LCylinder::vbo;

LGraphics::LCylinder::LCylinder(LGraphics::LApp* app, LGraphics::LImage::ImageResource res)
    :LImagedShape(app, res, objChanged, uniforms, needToResetBuffer)
{
    LOG_CALL
    setBuffer(app->cylinder->getMehes()[0].buffer);
}

void LGraphics::LCylinder::drawInstanced()
{
    LImagedShape::drawInstanced(L_CYLINDER, objChanged, needToResetBuffer, app->cylinder->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->cylinder->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LCylinder::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LCylinder::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LCylinder::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LCylinder::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LCylinder::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LCylinder::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LCylinder::updateBuffer()
{
    LImagedShape::updateBuffer(L_CYLINDER, objChanged, needToResetBuffer, app->cylinder->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LCylinder::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LCylinder::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->cylinder->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
