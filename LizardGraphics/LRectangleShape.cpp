#include "pch.h"
#include "LRectangleShape.h"
#include "LBaseComponent.h"
#include "LApp.h"

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

    bool LRectangleShape::mouseOnIt()
    {
        float x_[4], y_[4];
        auto vbo = buffer->getVertices();

        // getting rectangle coordinates 
        for (size_t i = 0; i < buffer->getVerticesCount(); ++i)
        {
            x_[i] = (vbo[i * 3] * getScale().x) + getMove().x;
            y_[i] = (vbo[i * 3 + 1] * getScale().y) + getMove().y;
        }

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->getWindowHandler(), &mouse_x, &mouse_y);
        return isPointInPolygon(buffer->getVerticesCount(), x_, y_, pointOnDisplayToGlCoords(app->getWindowSize(), { (float)mouse_x ,(float)mouse_y }));
    }

    void LRectangleShape::draw()
    {
        //if (dynamic_cast<LSymbol*>(this))
            //dynamic_cast<LSymbol*>(this)->setTextureBuffer();
        auto shader = getShader();
        shader->use();
        glBindTexture(GL_TEXTURE_2D, getTexture());
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        glBindVertexArray(buffer->getVaoNum());
        glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}