#include "pch.h"
#include "LRectangleShape.h"
#include "LBaseComponent.h"
#include "LApp.h"
#include "LIButton.h"

namespace LGraphics
{
    LRectangleShape::LRectangleShape(LApp* app, LObject* parent, const char* path, LBaseComponent* component)
        :LShape(path, component)
    {
        this->app = app;
        app->addObject(this);
    }

    LRectangleShape::LRectangleShape(LApp * app, LObject * parent, const unsigned char * bytes, size_t size, LBaseComponent* component)
        :LShape(bytes, size, component)
    {
        this->app = app;
        app->addObject(this);
    }

    void LRectangleShape::setLabel(const std::string label)
    {
        fvect2 coords = { getBottomLeftCorner().x , getBottomLeftCorner().y};
        this->label.pos = { coords.x + labelTextStartPosition, coords.y};
        this->label.text = label;
        alignLabel();
    }

    void LRectangleShape::alignLabel()
    {
        if (label.text.empty())
            return;
        float widgetLength = calculateWidgetLength();
        float textLength = LLine::getTextLength(label);

        if (textLength > widgetLength)
            scaleWithoutAlign({ ((getScale().x * textLength)/widgetLength) + 0.05f ,getScale().y,getScale().z });
        widgetLength = calculateWidgetLength();
        labelTextStartPosition = widgetLength >= textLength ? (widgetLength - textLength) / 2.0f : 0.0f;
        updateLabelPos();
    }

    float LRectangleShape::calculateWidgetLength()
    {
        return xGlCoordToScreenCoord(app->getWindowSize(), getTopRightCorner().x) - xGlCoordToScreenCoord(app->getWindowSize(), getTopLeftCorner().x);
    }

    fvect3 LRectangleShape::getCenter() const
    {
        return ((((LRectangleBuffer*)buffer)->getTopLeftCorner() * scale_ + move_) + (((LRectangleBuffer*)buffer)->getBottomRightCorner() * scale_ + move_)) /2;
    }

    fvect3 LRectangleShape::getTopLeftCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getTopLeftCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getTopRightCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getTopRightCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getBottomLeftCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getBottomLeftCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getBottomRightCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getBottomRightCorner() * scale_ + move_;
    }

    void LRectangleShape::updateLabelPos()
    {
        fvect2 coords = { getBottomLeftCorner().x, getBottomLeftCorner().y };
        this->label.pos = { coords.x + labelTextStartPosition/app->getWindowSize().x, coords.y };
    }

    bool LRectangleShape::mouseOnIt()
    {
        float x_[4], y_[4];
        float* vbo = buffer->getVertices();

        // getting rectangle coordinates 
        for (size_t i = 0; i < buffer->getVerticesCount(); ++i)
        {
            x_[i] = vbo[i * 3] * getScale().x + getMove().x;
            y_[i] = vbo[i * 3 + 1] * getScale().y + getMove().y;
        }

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->getWindowHandler(), &mouse_x, &mouse_y);

        return isPointInPolygon((int)buffer->getVerticesCount(), x_, y_, pointOnScreenToGlCoords(app->getWindowSize(), { (float)mouse_x ,(float)mouse_y }));
    }

    void LRectangleShape::draw()
    {
        auto shader = getShader();
        shader->use();
        if (isTextureTurnedOn()) glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 1);
        else glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, getTexture());
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        glBindVertexArray(buffer->getVaoNum());
        glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        LLine::display(label);
    }
}