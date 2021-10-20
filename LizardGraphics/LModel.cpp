#include "LModel.h"
#include "LModel.h"
#include "LModel.h"
//#include "LModelBuffer.h"
#include "LResourceManager.h"
#include "LLogger.h"
#include "LApp.h"
//#include "LBuffer.h"

void LGraphics::LModel::setDiffuse(const TexturesData& data, size_t meshNum)
{
    auto& diff = TO_GL(meshes[meshNum].image->textures[0]);
    const auto& in = TO_GL(data);
    diff.id = in.id;
    diff.offset = in.offset;
    diff.size = in.size;
}

void LGraphics::LModel::setNormal(const TexturesData& data, size_t meshNum)
{
    auto& norm = TO_GL(meshes[meshNum].image->textures[1]);
    const auto& in = TO_GL(data);
    norm.id = in.id;
    norm.offset = in.offset;
    norm.size = in.size;
}

void LGraphics::LModel::setDisplacement(const TexturesData& data, size_t meshNum)
{
    auto& disp = TO_GL(meshes[meshNum].image->textures[2]);
    const auto& in = TO_GL(data);
    disp.id = in.id;
    disp.offset = in.offset;
    disp.size = in.size;
}


void LGraphics::LModel::setNormalMapping(bool normalMapping, size_t meshNum)
{
    meshes[meshNum].image->setNormalMapping(normalMapping);
}

void LGraphics::LModel::setParallaxMapping(bool parallaxMapping, size_t meshNum)
{
    meshes[meshNum].image->setParallaxMapping(parallaxMapping);
}

void LGraphics::LModel::setNormalMappingAllMeshes(bool normalMapping)
{
    for (size_t i = 0; i < meshes.size(); ++i)
        meshes[i].image->setNormalMapping(normalMapping);
}

void LGraphics::LModel::setParallaxMappingAllMeshes(bool parallaxMapping)
{
    for (size_t i = 0; i < meshes.size(); ++i)
        meshes[i].image->setParallaxMapping(parallaxMapping);
}

LGraphics::LModel::LModel(LApp* app, ModelResource modelResource)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, modelResource);
    setShader(app->modelShader.get());
    app->toCreateM.push(this);
#ifdef VULKAN
    shader = app->lightShader.get();
#endif
    rotateX(180.0f);
}

LGraphics::LModel::LModel(LApp* app, const std::string& modelName, const std::string& diffuseName, 
    const std::string& normalsName, const std::string& displacementName)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, ModelResource{ modelName });

    setDiffuse(LResourceManager::loadMaterialTextures(app->getRealDiffusePath() + diffuseName));
    setNormal(LResourceManager::loadMaterialTextures(app->getRealNormalPath() + normalsName));
    setDisplacement(LResourceManager::loadMaterialTextures(app->getRealDisplacementPath() + displacementName));

    setNormalMappingAllMeshes(normalsName.size());
    setParallaxMappingAllMeshes(displacementName.size());

    setShader(app->modelShader.get());
    app->toCreateM.push(this);
#ifdef VULKAN
    shader = app->lightShader.get();
#endif
    rotateX(180.0f);
}

LGraphics::LModel::LModel(LApp* app, const std::string& modelName, const std::vector<std::string>& diffuseNames, 
    const std::vector<std::string>& normalNames, const std::vector<std::string>& displacementNames)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, { modelName });

    for (size_t i = 0; i < diffuseNames.size(); ++i)
    {
        if (i < diffuseNames.size())
            setDiffuse(LResourceManager::loadMaterialTextures(app->getRealDiffusePath()+ diffuseNames[i]),i);
        if (i < normalNames.size())
            setNormal(LResourceManager::loadMaterialTextures(app->getRealNormalPath() + normalNames[i]), i);
        if (i < displacementNames.size())
            setDisplacement(LResourceManager::loadMaterialTextures(app->getRealDisplacementPath() + displacementNames[i]), i);
    }
    setShader(app->modelShader.get());
    app->toCreateM.push(this);
#ifdef VULKAN
    shader = app->lightShader.get();
#endif
    rotateX(180.0f);
}

LGraphics::LModel::LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices)
    :LShape(app)
{
    res.normals = false;
    setShader(app->modelShader.get());
    std::vector<uint32_t> dummy;
    meshes = { {new LBuffer(app, vertices, dummy), new LImage(res, app->info.api)}};
    app->toCreateM.push(this);
    rotateX(180.0f);
}

LGraphics::LModel::LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    :LShape(app)
{
    res.normals = false;
    setShader(app->modelShader.get());
    meshes = { {new LBuffer(app, vertices, indices), new LImage(res, app->info.api)} };
    app->toCreateM.push(this);
    rotateX(180.0f);
}

LGraphics::LModel::~LModel()
{
    LOG_CALL
    for (auto& m : meshes)
    {
        if (m.buffer)
            delete m.buffer;
        if (m.image)
            delete m.image;
    }
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
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model_"), 1, GL_FALSE, glm::value_ptr(model));
    FOR(i, 0, meshes.size())
    {
        if (meshes[i].image)
        {
            if (!app->drawingInShadow)
            {
#ifdef MEGATEXTURE_LG
                auto& diffuse = meshes[i].image->getDiffuse();
                const auto castedDiff = TO_GL(diffuse);
                auto& normal = meshes[i].image->getNormal();
                const auto castedNorm = TO_GL(normal);
                auto& parallax = meshes[i].image->getParallax();
                const auto castedParallax = TO_GL(parallax);

                glUniform2f(glGetUniformLocation(shaderProgram, "offset"), castedDiff.offset.x, castedDiff.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSize"), castedDiff.size.x, castedDiff.size.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "offsetNormal"), castedNorm.offset.x, castedNorm.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSizeNormal"), castedNorm.size.x, castedNorm.size.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "offsetParallax"), castedParallax.offset.x, castedParallax.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSizeParallax"), castedParallax.size.x, castedParallax.size.y);
                glUniform1i(glGetUniformLocation(shaderProgram, "normalMapping"), getNormalMapping(i));
                glUniform1i(glGetUniformLocation(shaderProgram, "parallaxMapping"), getParallaxMapping(i));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.idNormal);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.idParallax);
#endif
            }
        }

        glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
        glActiveTexture(GL_TEXTURE2);
        glBindVertexArray(meshes[i].buffer->getVaoNum());
        if (meshes[i].buffer->getIndices().size())
            glDrawElements(GL_TRIANGLES, meshes[i].buffer->getIndices().size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, meshes[i].buffer->getVertices().size());
        glBindVertexArray(0);
    }
}

LGraphics::LModel::LModel(LApp* app, const std::string& path, size_t dummy)
    :LShape(app)
{
    std::string pathStr(path);
    LResourceManager::loadModel(this, pathStr);
    setShader(app->modelShader.get());
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
