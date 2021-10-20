#include "LSphere.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LSphere::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LSphere::objChanged;
bool LGraphics::LSphere::needToResetBuffer = false;
GLuint LGraphics::LSphere::vbo;

LGraphics::LSphere::LSphere(LApp* app, ImageResource res)
    :LImagedShape(app, res, objChanged, uniforms, needToResetBuffer)
{
    LOG_CALL
        setBuffer(app->sphere->getMehes()[0].buffer);
}

void LGraphics::LSphere::drawInstanced()
{
    LImagedShape::drawInstanced(L_SPHERE, objChanged, needToResetBuffer, app->sphere->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->sphere->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LSphere::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LSphere::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LSphere::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LSphere::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LSphere::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LSphere::setNormalMapping(bool normalMapping)
{
    LImagedShape::setNormalMapping(objChanged, this, normalMapping);
}

void LGraphics::LSphere::setParallaxMapping(bool parallaxMapping)
{
    LImagedShape::setParallaxMapping(objChanged, this, parallaxMapping);
}

void LGraphics::LSphere::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LSphere::updateBuffer()
{
    LImagedShape::updateBuffer(L_SPHERE, objChanged, needToResetBuffer, app->sphere->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LSphere::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LSphere::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->sphere->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
