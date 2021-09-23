#include "pch.h"
#include "LWRectangle.h"
#include "LRectangleBuffer.h"
#include "LApp.h"
#include "additional.h"
#include "LLogger.h"

#include "include/glm/gtc/type_ptr.hpp"

std::vector<LGraphics::LWidget::RectangleUniforms> LGraphics::LWRectangle::uniforms;
bool LGraphics::LWRectangle::needToResetBuffer;
GLuint LGraphics::LWRectangle::vbo;
std::vector<LGraphics::LWRectangle*> LGraphics::LWRectangle::objChanged;

LGraphics::LWRectangle::LWRectangle(LApp* app, const char* path, bool notRectangle)
    :LRectangleShape(app, path, false)
{
    LOG_CALL
    objChanged.push_back(this);
    if (uniforms.size() == uniforms.capacity())
        needToResetBuffer = true;
    uniforms.push_back(RectangleUniforms());
}

//glm::vec4 LGraphics::LWRectangle::getScreenCoords() const
//{
//    glm::vec4 coords_ = glm::vec4{
//    (((LRectangleBuffer*)buffer)->getBottomLeftCorner().x + ((LRectangleBuffer*)buffer)->getBottomRightCorner().x)/2.0f,
//    (((LRectangleBuffer*)buffer)->getTopRightCorner().y + ((LRectangleBuffer*)buffer)->getBottomRightCorner().y) / 2.0f,
//    0.0f, 1.0f};
//    return projection * app->getViewMatrix() * calculateModelMatrix() * coords_;
//}

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
    //if (isHidden())
    //    return;

    //auto shader = (LShaders::OpenGLShader*)getShader();
    //if (app->drawingInShadow)
    //    shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    //GLuint shaderProgram = shader->getShaderProgram();

    //const auto proj = app->getProjectionMatrix();
    //const auto view = app->getViewMatrix();
    //const auto model = calculateModelMatrix();

    //shader->use();
    //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));
    //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
    //glUniform1i(glGetUniformLocation(shaderProgram, "objId"), id);

    //if (!app->drawingInShadow)
    //{
    //    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
    //    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

    //    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    //    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    //    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
    //    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
    //    glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
    //}

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, *(GLuint*)getTexture());
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, app->depthMap);
    //glBindVertexArray(buffer->getVaoNum());
    //glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_SHORT, 0);
}

void LGraphics::LWRectangle::drawInstanced()
{
    if (!uniforms.size()) return;
    auto shader = (LShaders::OpenGLShader*)app->getStandartShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);
    updateBuffer();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->depthMap);
    glBindVertexArray(app->standartRectBuffer->getVaoNum());
    glDrawElementsInstanced(GL_TRIANGLES, app->standartRectBuffer->getIndCount(), GL_UNSIGNED_SHORT, 0, uniforms.size());
    //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, uniforms.size());
}

void LGraphics::LWRectangle::setModel(const glm::mat4& model)
{
    LShape::setModel(model);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::scale(const float x, const float y, const float z)
{
    LShape::scale(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::move(const float x, const float y, const float z)
{
    LShape::move(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::setDiffuse(const char* path)
{
    texturePath = path;
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::rotateX(float angle)
{
    LWidget::rotateX(angle);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::rotateY(float angle)
{
    LWidget::rotateY(angle);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::rotateZ(float angle)
{
    LWidget::rotateZ(angle);
    objChanged.push_back(this);
}

void LGraphics::LWRectangle::updateBuffer()
{
    if (objChanged.size())
    {
        if (needToResetBuffer)
        {
            resetInstanceBuffer();
            needToResetBuffer = false;
        }
        LWidget::CubeUniforms* buffer = (LWidget::RectangleUniforms*)glMapNamedBufferRange(vbo, 0, sizeof(LWidget::RectangleUniforms) * uniforms.size(),
            GL_MAP_WRITE_BIT);

#ifdef PARALLEL_UPDATE

        if (objChanged.size() > 1000 && app->info.freeThreads > 1)
        {
            std::thread* threads = new std::thread[app->info.freeThreads];
            for (size_t i = 0; i < app->info.freeThreads; ++i)
            {
                const size_t interval = i * objChanged.size() / app->info.freeThreads;
                threads[i] = std::thread(LWRectangle::updateBufferParallel, buffer, objChanged, interval, interval + objChanged.size() / app->info.freeThreads);
            }

            for (size_t i = 0; i < app->info.freeThreads; ++i)
                threads[i].join();
            delete[] threads;
        }
        else
#endif
            for (const auto& obj : objChanged)
                updateUniforms(buffer, obj->id);
        objChanged.clear();
        glUnmapNamedBuffer(vbo);
    }
}

void LGraphics::LWRectangle::updateUniforms(LWidget::RectangleUniforms* buffer, size_t id)
{
    auto p = ((LWRectangle*)app->primitives[L_RECTANGLE][id])->getTexturePath();
    const auto subtexture = app->megatexture.subtextures[((LWRectangle*)app->primitives[L_RECTANGLE][id])->getTexturePath()];
    auto& obj = buffer[id];
    obj.model = ((LWRectangle*)app->primitives[L_RECTANGLE][id])->calculateModelMatrix();
    obj.offset = subtexture.first;
    obj.textureSize = subtexture.second;
}

void LGraphics::LWRectangle::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LWRectangle::resetInstanceBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::RectangleUniforms) * uniforms.capacity(), uniforms.data(), GL_STATIC_DRAW);

    glBindVertexArray(app->standartRectBuffer->getVaoNum());
    const size_t vec4Size = sizeof(glm::vec4);
    const size_t vec2Size = sizeof(glm::vec2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)(3 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)(4 * vec4Size));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::RectangleUniforms), (void*)((4 * vec4Size) + vec2Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glBindVertexArray(0);
}

void LGraphics::LWRectangle::updateBufferParallel(LWidget::RectangleUniforms* buffer, std::vector<LWRectangle*>& changed, size_t begin, size_t end)
{
    for (size_t i = begin; i < end; i++)
        updateUniforms(buffer, changed[i]->id);
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
