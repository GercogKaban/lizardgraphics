#include "LTorus.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LTorus::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LTorus::objChanged;
bool LGraphics::LTorus::needToResetBuffer = false;
GLuint LGraphics::LTorus::vbo;

LGraphics::LTorus::LTorus(LApp* app, ImageResource res)
    :LImagedShape(app, res, objChanged, uniforms, needToResetBuffer)
{
    LOG_CALL
    setBuffer(app->torus->getMehes()[0].buffer);
}

void LGraphics::LTorus::drawInstanced()
{
    LImagedShape::drawInstanced(L_TORUS, objChanged, needToResetBuffer, app->torus->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->torus->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LTorus::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LTorus::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LTorus::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LTorus::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LTorus::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LTorus::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LTorus::setNormalMapping(bool normalMapping)
{
    LImagedShape::setNormalMapping(objChanged, this, normalMapping);
}

void LGraphics::LTorus::setParallaxMapping(bool parallaxMapping)
{
    LImagedShape::setParallaxMapping(objChanged, this, parallaxMapping);
}


void LGraphics::LTorus::updateBuffer()
{
    LImagedShape::updateBuffer(L_TORUS, objChanged, needToResetBuffer, app->torus->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LTorus::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LTorus::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->torus->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
