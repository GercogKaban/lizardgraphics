#include "LWRectangle.h"
#include "LBuffer.h"
#include "LApp.h"

#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LWRectangle::LWRectangle(LApp * app, const char * path, bool lazy)
    :LRectangleShape(app, path, lazy)
{
    shader = app->getStandartWorldObjShader();
    view = app->getViewMatrix();
    projection = app->getProjectionMatrix();
}

void LGraphics::LWRectangle::setMatrices(LApp* app)
{
    view = app->getViewMatrix();
    projection = app->getProjectionMatrix();
}

void LGraphics::LWRectangle::setMatrices()
{
    view =
        glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    auto aspect = (float)app->getWindowSize().x / (float)app->getWindowSize().y;
    projection = glm::ortho(-1.0f, 1.0f, -1.0f * aspect, 1.0f * aspect, 0.1f, 100.0f);
}

void LGraphics::LWRectangle::draw()
{
    getShader()->use();
    auto shader = getShader()->getShaderProgram();
    if (isTextureTurnedOn()) glUniform1i(glGetUniformLocation(shader, "sampleTexture"), 1);
    else glUniform1i(glGetUniformLocation(shader, "sampleTexture"), 0);
    glBindTexture(GL_TEXTURE_2D, getTexture());

    model = glm::scale(model, glm::vec3(scale_.x, scale_.y, scale_.z));
    model = glm::translate(model, glm::vec3(move_.x, move_.y, move_.z));

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "color_"), color_.x, color_.y, color_.z, transparency_);
    glBindVertexArray(buffer->getVaoNum());
    glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    LLine::display(label);
}
