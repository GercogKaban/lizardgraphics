#include "LCube.h"
#include "LApp.h"
#include "LModel.h"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LCube::uniforms;
std::vector<LGraphics::LShape*> LGraphics::LCube::objChanged;
bool LGraphics::LCube::needToResetBuffer = false;
GLuint LGraphics::LCube::vbo;

LGraphics::LCube::LCube(LApp* app, ImageResource res)
	:LImagedShape(app, res,objChanged,uniforms,needToResetBuffer)
{
    LOG_CALL
    setBuffer(app->cube->getMehes()[0].buffer);
}

void LGraphics::LCube::drawInstanced()
{
    LImagedShape::drawInstanced(L_CUBE, objChanged, needToResetBuffer, app->cube->getMehes()[0].buffer->getVaoNum(),
        vbo, uniforms, app->cube->getMehes()[0].buffer->getIndices().size());
}

void LGraphics::LCube::setModel(const glm::mat4& model)
{
    LImagedShape::setModel(objChanged, this, model);
}

void LGraphics::LCube::scale(const float x, const float y, const float z)
{
    LImagedShape::scale(objChanged, this, x, y, z);
}

void LGraphics::LCube::move(const float x, const float y, const float z)
{
    LImagedShape::move(objChanged, this, x, y, z);
}

void LGraphics::LCube::rotateX(float angle)
{
    LImagedShape::rotateX(objChanged, this, angle);
}

void LGraphics::LCube::rotateY(float angle)
{
    LImagedShape::rotateY(objChanged, this, angle);
}

void LGraphics::LCube::rotateZ(float angle)
{
    LImagedShape::rotateZ(objChanged, this, angle);
}

void LGraphics::LCube::setNormalMapping(bool normalMapping)
{
    LImagedShape::setNormalMapping(objChanged, this, normalMapping);
}

void LGraphics::LCube::setParallaxMapping(bool parallaxMapping)
{
    LImagedShape::setParallaxMapping(objChanged, this, parallaxMapping);
}

void LGraphics::LCube::updateBuffer()
{
    LImagedShape::updateBuffer(L_CUBE,objChanged, needToResetBuffer, app->cube->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}

void LGraphics::LCube::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LCube::resetInstanceBuffer()
{
    LImagedShape::resetInstanceBuffer(app->cube->getMehes()[0].buffer->getVaoNum(), vbo, uniforms);
}
