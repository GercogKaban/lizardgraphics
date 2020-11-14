#include "LWRectangle.h"
#include "LRectangleBuffer.h"
#include "LApp.h"
#include "additional.h"

//#include <iostream>

//#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LWRectangle::LWRectangle(LApp * app, const char * path)
    :LRectangleShape(app, path, false)
{
    shader = app->getStandartWorldObjShader();
    //view = app->getViewMatrix();
    projection = app->getProjectionMatrix();
}

glm::vec2 LGraphics::LWRectangle::getScreenCoords() const
{
    glm::vec4 coords_ = glm::vec4{
    (((LRectangleBuffer*)buffer)->getBottomLeftCorner().x + ((LRectangleBuffer*)buffer)->getBottomRightCorner().x)/2.0f,
    (((LRectangleBuffer*)buffer)->getTopRightCorner().y + ((LRectangleBuffer*)buffer)->getBottomRightCorner().y) / 2.0f,
    0.0f, 1.0f};
    return projection * app->getViewMatrix() * calculateModelMatrix() * coords_;
}

void LGraphics::LWRectangle::setMatrices(LApp* app)
{
    //view = app->getViewMatrix();
    projection = app->getProjectionMatrix();
}

void LGraphics::LWRectangle::setMatrices()
{
    //view =
    //    glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
    //        glm::vec3(0.0f, 0.0f, 0.0f),
    //        glm::vec3(0.0f, 1.0f, 0.0f));

    auto aspect = (float)app->getWindowSize().x / (float)app->getWindowSize().y;
    //projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    projection = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, 0.1f, 1000.0f);
}

void LGraphics::LWRectangle::draw()
{
    if (isHidden()) return;

    getShader()->use();
    auto shader = getShader()->getShaderProgram();
    glUniform1i(glGetUniformLocation(shader, "sampleTexture"), isTextureTurnedOn());
    glUniform1i(glGetUniformLocation(shader, "isometric"), isometric);

    double xpos, ypos;
    glfwGetCursorPos(app->getWindowHandler(), &xpos, &ypos);
    glBindVertexArray(buffer->getVaoNum());

    auto camera = app->getViewPoint();
    // test
    //int data = -1;

    //GLuint ssbo;
    //glGenBuffers(1, &ssbo);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, 4, &data, GL_STATIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, getTexture());

    model = calculateModelMatrix();
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(move_.x, move_.y, move_.z));
    //model = glm::rotate(model, glm::radians(rotateX_), { 1.0f,0.0f,0.0f });
    //model = glm::rotate(model, glm::radians(rotateY_), { 0.0f,1.0f,0.0f });
    //model = glm::rotate(model, glm::radians(rotateZ_), { 0.0f,0.0f,1.0f });
    //model = glm::scale(model, glm::vec3(scale_.x, scale_.y, scale_.z));
    //model = glm::translate(model, glm::vec3(move_.x, move_.y, move_.z));

    //glUniform2f(glGetUniformLocation(shader, "mouse"), xpos, ypos);
    //glUniform2f(glGetUniformLocation(shader, "resolution"), app->getWindowSize().x, app->getWindowSize().y);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(app->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "color_"), color_.x, color_.y, color_.z, transparency_);

    glUniform3f(glGetUniformLocation(shader, "lightPos"), camera.x, camera.y, camera.z);
    glBindVertexArray(buffer->getVaoNum());

    glBindVertexArray(buffer->getVaoNum());
    glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    //memcpy(&data, p, sizeof(data));
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //std::cout << data << std::endl;

    //glBindVertexArray(0);
    //(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    glBindVertexArray(0);

    for (auto& i : innerWidgets)
        i->draw();
    //LLine::display(label);
}

glm::mat4 LGraphics::LWRectangle::calculateModelMatrix() const
{
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
    model_ = glm::rotate(model_, glm::radians(rotateX_), { 1.0f,0.0f,0.0f });
    model_ = glm::rotate(model_, glm::radians(rotateY_), { 0.0f,1.0f,0.0f });
    model_ = glm::rotate(model_, glm::radians(rotateZ_), { 0.0f,0.0f,1.0f });
    model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
    return model_;
}
