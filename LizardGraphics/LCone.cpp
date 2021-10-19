#include "LCone.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LCone::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LCone::objChanged;
bool LGraphics::LCone::needToResetBuffer = false;
GLuint LGraphics::LCone::vbo;

LGraphics::LCone::LCone(LApp* app, ImageResource res)
    :LImagedShape(app, res, objChanged, uniforms, needToResetBuffer)
{
    LOG_CALL
        setBuffer(app->cone->getMehes()[0].buffer);
}

void LGraphics::LCone::drawInstanced()
{
    LImagedShape::drawInstanced(L_CONE, objChanged, needToResetBuffer, app->cone->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->cone->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LCone::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LCone::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LCone::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LCone::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LCone::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LCone::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LCone::updateBuffer()
{
    LImagedShape::updateBuffer(L_CONE, objChanged, needToResetBuffer, app->cone->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LCone::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LCone::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->cone->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
