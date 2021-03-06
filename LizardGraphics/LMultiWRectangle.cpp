#include "pch.h"

#include "LMultiWRectangle.h"
#include "LMultiRectangleBuffer.h"
#include "LLogger.h"
//#include "LApp.h"
#include "additional.h"

//#include <iostream>

//#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LMultiWRectangle::LMultiWRectangle(LApp* app, size_t rectCount)
    : LWidgetGroup<LWRectangle>(app), rectCount(rectCount)
{
    LOG_CALL
    //shader = app->multi_defaultShader;
    view = app->getViewMatrix();
    projection = app->getProjectionMatrix();

    buffer = new LMultiRectangleBuffer(rectCount);
}

//glm::vec2 LGraphics::LWRectangle::getScreenCoords() const
//{
//    glm::vec4 coords_ = glm::vec4{
//    (((LRectangleBuffer*)buffer)->getBottomLeftCorner().x + ((LRectangleBuffer*)buffer)->getBottomRightCorner().x)/2.0f,
//    (((LRectangleBuffer*)buffer)->getTopRightCorner().y + ((LRectangleBuffer*)buffer)->getBottomRightCorner().y) / 2.0f,
//    0.0f, 1.0f};
//    return projection * app->getViewMatrix() * calculateModelMatrix() * coords_;
//}

void LGraphics::LMultiWRectangle::draw()
{
    LOG_CALL
    //if (isHidden()) return;

    if (!widgets.size())
        return;

    //this->shader->use();
#ifdef OPENGL
    auto shader = this->shader->getShaderProgram();
    auto lightPos = app->getLightPos();

    textures.clear();
    for (auto& r : widgets)
        textures.push_back(r->getTexture());

    char buf[16];
    strcpy(buf, "ourTexture_");

    for (size_t i = 0; i < rectCount; i++)
    {
        buf[11] = (i + 1) / 10 + '0';
        buf[12] = (i+1) % 10 + '0';
        buf[13] = 0;

        glUniform1i(glGetUniformLocation(shader, buf), i);

        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }


    glUniform1i(glGetUniformLocation(shader, "shadowMap"), 0);

    //glUniform1i(glGetUniformLocation(shader, "sampleTexture"), isTextureTurnedOn());
    //glUniform1i(glGetUniformLocation(shader, "isometric"), isometric);

    glUniformMatrix4fv(glGetUniformLocation(shader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->getLightSpaceMatrix()));
    glUniform3f(glGetUniformLocation(shader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // test
    //int data = -1;

    //GLuint ssbo;
    //glGenBuffers(1, &ssbo);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, 4, &data, GL_STATIC_DRAW);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->getDepthMap());


    const float t = sqrt(3);
    const glm::vec3 viewPos = app->getViewPoint() + app->getViewRadius() * glm::vec3(t, t, t);
    glUniform3f(glGetUniformLocation(shader, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
   
    models.clear();
    for (auto& r : widgets)
        models.push_back(r->getModelMatrix());
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), rectCount, GL_FALSE, (GLfloat*)(&models[0]));
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(app->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    //glUniform4f(glGetUniformLocation(shader, "color_"), color_.x, color_.y, color_.z, transparency_);


    glBindVertexArray(buffer->getVaoNum());
    glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    //memcpy(&data, p, sizeof(data));
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //std::cout << data << std::endl;

    //glBindVertexArray(0);
    //(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    //glBindVertexArray(0);
#endif OPENGL
    //for (auto& i : innerWidgets)
    //    i->draw();
}

//glm::mat4 LGraphics::LWRectangle::calculateModelMatrix() const
//{
//    glm::mat4 model_ = glm::mat4(1.0f);
//    model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
//    model_ *= rotate_;
//    model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
//    return model_;
//}
