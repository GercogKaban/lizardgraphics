#include "pch.h"
#include "LWRectangle.h"
#include "LRectangleBuffer.h"
#include "LApp.h"
#include "additional.h"
#include "LLogger.h"

#include "include/glm/gtc/type_ptr.hpp"

std::vector<LGraphics::LWidget::RectangleUniforms> LGraphics::LWRectangle::uniforms;

LGraphics::LWRectangle::LWRectangle(LApp* app, const char* path, bool notRectangle)
    :LRectangleShape(app, path, false)
{
    LOG_CALL
    shader = app->getLightningShader().get();
    projection = app->getProjectionMatrix();
    app->toCreate.push(this);
}

glm::vec4 LGraphics::LWRectangle::getScreenCoords() const
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

    //auto aspect = (float)app->getWindowSize().x / (float)app->getWindowSize().y;

    //projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //projection = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, 0.1f, 1000.0f);
}

void LGraphics::LWRectangle::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
    LOG_CALL
    app->updateUniformBuffer(frameIndex, arrayIndex, this);

    const uint32_t dynamicOffsets[] = 
    { arrayIndex * static_cast<uint32_t>(app->dynamicAlignment),
    };

     vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
         ((LShaders::VulkanShader*)getShader())->getPipelineLayout(), 0, 1, &app->descriptorSets[arrayIndex * 2 + frameIndex], 1, dynamicOffsets);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(buffer->getIndCount()), 1, 0, 0, 0);
}

void LGraphics::LWRectangle::draw()
{
    if (isHidden())
        return;

    auto shader = (LShaders::OpenGLShader*)getShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();

    const auto proj = app->getProjectionMatrix();
    const auto view = app->getViewMatrix();
    const auto model = calculateModelMatrix();

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
    glUniform1i(glGetUniformLocation(shaderProgram, "objId"), id);

    if (!app->drawingInShadow)
    {
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *(GLuint*)getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->depthMap);
    glBindVertexArray(buffer->getVaoNum());
    glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_SHORT, 0);
}

void LGraphics::LWRectangle::drawInstanced()
{
}

//glm::mat4 LGraphics::LWRectangle::calculateModelMatrix() const
//{
//#if _DEBUG
//#include "../Optimized/optimized.h"
//    return _calculateModelMatrix(move_, rotate_, scale_);
//#else
//    glm::mat4 model_ = glm::mat4(1.0f);
//    model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
//    model_ *= rotate_;
//    model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
//    return model_;
//#endif
//}
