#include "pch.h"
#include "LPlane.h"
#include "LApp.h"
//#include "additional.h"
#include "LLogger.h"
#include "LBuffer.h"
#include "LModel.h"

#include "include/glm/gtc/type_ptr.hpp"

std::vector<LGraphics::LWidget::PrimitiveUniforms> LGraphics::LPlane::uniforms;
std::vector<LGraphics::LPlane*> LGraphics::LPlane::objChanged;
bool LGraphics::LPlane::needToResetBuffer = false;
GLuint LGraphics::LPlane::vbo;

LGraphics::LPlane::LPlane(LApp* app, ImageResource res)
    :LRectangleShape(app, res)
{
    LOG_CALL
    objChanged.push_back(this);
    if (uniforms.size() == uniforms.capacity())
        needToResetBuffer = true;
    uniforms.push_back(PrimitiveUniforms());
    setBuffer(app->plane->getMehes()[0].buffer);
}

void LGraphics::LPlane::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
    LOG_CALL
    app->updateUniformBuffer(frameIndex, arrayIndex, this);

    const uint32_t dynamicOffsets[] = 
    { arrayIndex * static_cast<uint32_t>(app->dynamicAlignment),
    };

     vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
         ((LShaders::VulkanShader*)getShader())->getPipelineLayout(), 0, 1, &app->descriptorSets[arrayIndex * 2 + frameIndex], 1, dynamicOffsets);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(buffer->getIndices().size()), 1, 0, 0, 0);
}

// deprecated
void LGraphics::LPlane::draw()
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

void LGraphics::LPlane::drawInstanced()
{
    if (!uniforms.size()) return;
    auto shader = (LShaders::OpenGLShader*)app->getStandartShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);
    updateBuffer();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
    if (!app->drawingInShadow)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, app->megatexture.idNormal);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, app->megatexture.idParallax);
    }
    glBindVertexArray(app->plane->getMehes()[0].buffer->getVaoNum());
    glDrawElementsInstanced(GL_TRIANGLES, app->plane->getMehes()[0].buffer->getIndices().size(), GL_UNSIGNED_INT, 0, uniforms.size());
}

void LGraphics::LPlane::setModel(const glm::mat4& model)
{
    LShape::setModel(model);
    objChanged.push_back(this);
}

void LGraphics::LPlane::scale(const float x, const float y, const float z)
{
    LShape::scale(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LPlane::move(const float x, const float y, const float z)
{
    LShape::move(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LPlane::rotateX(float angle)
{
    LWidget::rotateX(angle);
    objChanged.push_back(this);
}

void LGraphics::LPlane::rotateY(float angle)
{
    LWidget::rotateY(angle);
    objChanged.push_back(this);
}

void LGraphics::LPlane::rotateZ(float angle)
{
    LWidget::rotateZ(angle);
    objChanged.push_back(this);
}

void LGraphics::LPlane::updateBuffer()
{
    if (objChanged.size())
    {
        if (needToResetBuffer)
        {
            resetInstanceBuffer();
            needToResetBuffer = false;
        }
        
        LWidget::PrimitiveUniforms* buffer = (LWidget::PrimitiveUniforms*)glMapNamedBufferRange(vbo, 0, sizeof(LWidget::PrimitiveUniforms) * uniforms.size(),
            GL_MAP_WRITE_BIT);
        if (!buffer)
            throw std::runtime_error(LOG_HEADER + "buffer == NULL, error: " + std::to_string(glGetError()) + "\n\n");
#ifdef PARALLEL_UPDATE

        if (objChanged.size() > 1000 && app->info.freeThreads > 1)
        {
            std::thread* threads = new std::thread[app->info.freeThreads];
            for (size_t i = 0; i < app->info.freeThreads; ++i)
            {
                const size_t interval = i * objChanged.size() / app->info.freeThreads;
                threads[i] = std::thread(LPlane::updateBufferParallel, buffer, objChanged, interval, interval + objChanged.size() / app->info.freeThreads);
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

void LGraphics::LPlane::updateUniforms(LWidget::PrimitiveUniforms* buffer, size_t id)
{
    auto path = ((LPlane*)app->primitives[L_PLANE][id])->getDiffusePath();
    auto pathNormal = ((LPlane*)app->primitives[L_PLANE][id])->getNormalsPath();
    auto pathDisplacement = ((LPlane*)app->primitives[L_PLANE][id])->getDisplacementPath();

    const auto subtexture = app->megatexture.subtextures[path];
    const auto subtextureNormal = app->megatexture.subtexturesNormal[pathNormal];
    const auto subtextureParallax = app->megatexture.subtexturesParallax[pathDisplacement];

    auto& obj = buffer[id];
    obj.model = ((LPlane*)app->primitives[L_PLANE][id])->calculateModelMatrix();
    obj.offset = subtexture.first;
    obj.textureSize = subtexture.second;
    obj.offsetNormal = subtextureNormal.first;
    obj.textureSizeNormal = subtextureNormal.second;
    obj.offsetParallax = subtextureParallax.first;
    obj.textureSizeParallax = subtextureParallax.second;
}

void LGraphics::LPlane::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LPlane::resetInstanceBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::PrimitiveUniforms) * uniforms.capacity(), uniforms.data(), GL_STATIC_DRAW);

    glBindVertexArray(app->plane->getMehes()[0].buffer->getVaoNum());
    const size_t vec4Size = sizeof(glm::vec4);
    const size_t vec2Size = sizeof(glm::vec2);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)0);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(2 * vec4Size));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(3 * vec4Size));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(4 * vec4Size));
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)((4 * vec4Size) + vec2Size));
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)((4 * vec4Size) + 2 * vec2Size));
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)((4 * vec4Size) + 3 * vec2Size));
    glEnableVertexAttribArray(13);
    glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)((4 * vec4Size) + 4 * vec2Size));
    glEnableVertexAttribArray(14);
    glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)((4 * vec4Size) + 5 * vec2Size));

    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);
    glVertexAttribDivisor(11, 1);
    glVertexAttribDivisor(12, 1);
    glVertexAttribDivisor(13, 1);
    glVertexAttribDivisor(14, 1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LGraphics::LPlane::updateBufferParallel(LWidget::PrimitiveUniforms* buffer, std::vector<LPlane*>& changed, size_t begin, size_t end)
{
    for (size_t i = begin; i < end; i++)
        updateUniforms(buffer, changed[i]->id);
}

//glm::mat4 LGraphics::LPlane::calculateModelMatrix() const
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
