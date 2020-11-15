#include "LApp.h"
#include "LBuffer.h"
#include "LColorBar.h"
#include "additional.h"
#include "pch.h"

LGraphics::LColorBar::LColorBar(LApp* app)
    :LRectangleShape(app,nullptr,true)
{
    setShader(app->colorBarShader);
    firstColor = rgbToFloat(0, 0, 0);
    secondColor = rgbToFloat(255, 0, 0);
    scale(0.5f, 0.05f, 1.0f);
    setFullness(0.5f);
}

void LGraphics::LColorBar::draw()
{
    if (isHidden()) return;

    auto shader = getShader();
    shader->use();

    glBindTexture(GL_TEXTURE_2D, getTexture());
    glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
    glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);

    glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "firstColor"), firstColor.x, firstColor.y, firstColor.z, 1.0f);
    glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "secondColor"), secondColor.x, secondColor.y, secondColor.z, 1.0f);
    glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "border"), border);

    glBindVertexArray(buffer->getVaoNum());

    glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    LLine::display(label);

    for (auto& i : innerWidgets)
        i->draw();
}

void LGraphics::LColorBar::setFirstColor(const unsigned char r, const unsigned char g, const unsigned char b)
{
    firstColor = rgbToFloat(r, g, b);
}

void LGraphics::LColorBar::setSecondColor(const unsigned char r, const unsigned char g, const unsigned char b)
{
    secondColor = rgbToFloat(r, g, b);
}

void LGraphics::LColorBar::setFullness(float fullness)
{
    auto dif = xGlCoordToScreenCoord(app->getWindowSize(), getBottomRightCorner().x)
        - xGlCoordToScreenCoord(app->getWindowSize(), getBottomLeftCorner().x);
    border = xGlCoordToScreenCoord(app->getWindowSize(), getBottomLeftCorner().x) + dif* fullness;
}