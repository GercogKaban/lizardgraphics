#include "LIcosphere.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LIcosphere::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LIcosphere::objChanged;
bool LGraphics::LIcosphere::needToResetBuffer = false;
GLuint LGraphics::LIcosphere::vbo;

LGraphics::LIcosphere::LIcosphere(LApp* app, ImageResource res)
    :LImagedShape(app, res, objChanged, uniforms, needToResetBuffer)
{
    LOG_CALL
    setBuffer(app->icosphere->getMehes()[0].buffer);
}

void LGraphics::LIcosphere::drawInstanced()
{
    LImagedShape::drawInstanced(L_ICOSPHERE, objChanged, needToResetBuffer, app->icosphere->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->icosphere->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LIcosphere::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LIcosphere::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LIcosphere::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LIcosphere::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LIcosphere::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LIcosphere::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LIcosphere::updateBuffer()
{
    LImagedShape::updateBuffer(L_ICOSPHERE, objChanged, needToResetBuffer, app->icosphere->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LIcosphere::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LIcosphere::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->icosphere->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
