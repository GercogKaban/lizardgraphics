#include "LModel.h"
//#include "LModelBuffer.h"
#include "LResourceManager.h"
#include "LLogger.h"
#include "LApp.h"
#include "LBuffer.h"

LGraphics::LModel::LModel(LApp* app, ModelResource modelResource)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, modelResource);
    setShader(app->modelShader.get());
    //if (texturePath)
    //{
    //    loadTexture(texturePath, BASE_TEXTURE);
    //    this->textureName = texturePath;
    //}
    //if (normalsPath)
    //    loadTexture(normalsPath, NORMALS);

    app->toCreate.push(this);
#ifdef VULKAN
    shader = app->lightShader.get();
#endif
}

LGraphics::LModel::~LModel()
{
    LOG_CALL
}

void LGraphics::LModel::draw()
{
    auto shader = (LShaders::OpenGLShader*)app->modelShader.get();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapModelShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);
    model = calculateModelMatrix();
    //if (!app->drawingInShadow)
    //    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model_"), 1, GL_FALSE, glm::value_ptr(model));

    FOR(i, 0, meshes.size())
    {
        //if (!app->drawingInShadow)
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model_"), 1, GL_FALSE, glm::value_ptr(model));
        auto& diffuse = meshes[i].image->getDiffuse();
        const auto castedDiff = TO_GL(diffuse);
#ifdef MEGATEXTURE_LG
        glUniform2f(glGetUniformLocation(shaderProgram, "offset"), castedDiff.offset.x, castedDiff.offset.y);
        glUniform2f(glGetUniformLocation(shaderProgram, "textureSize"), castedDiff.size.x, castedDiff.size.y);
#endif
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, castedDiff.id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
        glBindVertexArray(meshes[i].buffer->getVaoNum());
        glDrawElements(GL_TRIANGLES, meshes[i].buffer->getIndices().size(), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);
    }
}

//void LGraphics::LModel::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex)
//{
//    LOG_CALL
//    VkBuffer vertexBuffers[] = { buffer->getVertBuffer() };
//    VkDeviceSize offsets[] = { 0 };
//
//    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//    app->updateUniformBuffer(frameIndex, arrayIndex, this);
//
//    const uint32_t dynamicOffsets[] =
//    { arrayIndex * static_cast<uint32_t>(app->dynamicAlignment),
//    };
//
//    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//        ((LShaders::VulkanShader*)getShader())->getPipelineLayout(), 0, 1, &app->descriptorSets[arrayIndex * 2 + frameIndex], 1, dynamicOffsets);
//
//    for (size_t mesh = 0; mesh < ((LModelBuffer*)buffer)->getIndBuffer().size(); ++mesh)
//    {
//        if (!meshesToDraw[mesh])
//            continue;
//        vkCmdBindIndexBuffer(commandBuffer, ((LModelBuffer*)buffer)->getIndBuffer()[mesh], 0, VK_INDEX_TYPE_UINT32);
//        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(((LModelBuffer*)buffer)->indices[mesh].size()), 1, 0, 0, 0);
//    }
//}
