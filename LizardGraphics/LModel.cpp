#include "LModel.h"
#include "LResourceManager.h"
#include "LLogger.h"
#include "LApp.h"
#include "constants.h"

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

void LGraphics::LModel::setReflex(const TexturesData& data, size_t meshNum)
{
    auto& refl = TO_GL(meshes[meshNum].image->textures[3]);
    const auto& in = TO_GL(data);
    refl.id = in.id;
    refl.offset = in.offset;
    refl.size = in.size;
}

void LGraphics::LModel::setNormalMapping(bool normalMapping, size_t meshNum)
{
    meshes[meshNum].image->setNormalMapping(normalMapping);
}

void LGraphics::LModel::setParallaxMapping(bool parallaxMapping, size_t meshNum)
{
    meshes[meshNum].image->setParallaxMapping(parallaxMapping);
}

void LGraphics::LModel::setReflexMapping(bool reflexMapping, size_t meshNum)
{
    meshes[meshNum].image->setReflexMapping(reflexMapping);
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

void LGraphics::LModel::setReflexMappingAllMeshes(bool reflexMapping)
{
    for (size_t i = 0; i < meshes.size(); ++i)
        meshes[i].image->setReflexMapping(reflexMapping);
}

LGraphics::LModel::LModel(LApp* app, ModelResource modelResource, bool cropTextureCoords)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, modelResource, cropTextureCoords);
    init();
}

LGraphics::LModel::LModel(LApp* app, CTOR_PATH_VARS, bool cropTextureCoords)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, ModelResource{ modelName },cropTextureCoords);

    setDiffuse(LResourceManager::loadMaterialTextures(app->getRealDiffusePath() + diffuseName));
    setNormal(LResourceManager::loadMaterialTextures(app->getRealNormalPath() + normalName));
    setDisplacement(LResourceManager::loadMaterialTextures(app->getRealDisplacementPath() + displacementName));
    setReflex(LResourceManager::loadMaterialTextures(app->getRealReflexPath() + reflexName));

    setNormalMappingAllMeshes(normalName.size());
    setParallaxMappingAllMeshes(displacementName.size());
    setReflexMappingAllMeshes(reflexName.size());
    init();
}

LGraphics::LModel::LModel(LApp* app, CTOR_PATH_VARS_VEC, bool cropTextureCoords)
    :LShape(app)
{
    LOG_CALL
    LResourceManager::loadModel(this, { modelName },cropTextureCoords);

    for (size_t i = 0; i < diffuseNames.size(); ++i)
    {
        if (i < diffuseNames.size())
            setDiffuse(LResourceManager::loadMaterialTextures(app->getRealDiffusePath()+ diffuseNames[i]),i);
        if (i < normalNames.size())
            setNormal(LResourceManager::loadMaterialTextures(app->getRealNormalPath() + normalNames[i]), i);
        if (i < displacementNames.size())
            setDisplacement(LResourceManager::loadMaterialTextures(app->getRealDisplacementPath() + displacementNames[i]), i);
        if (i < reflexNames.size())
            setReflex(LResourceManager::loadMaterialTextures(app->getRealReflexPath() + reflexNames[i]), i);
    }
    init();
}

LGraphics::LModel::LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, bool cropTextureCoords)
    :LShape(app)
{
    res.normals = false;
    std::vector<uint32_t> dummy;
    meshes = { {new LBuffer(app, vertices, dummy), new LImage(res, app->info.api)}};
    init();
}

LGraphics::LModel::LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, 
    const std::vector<uint32_t>& indices)
    :LShape(app)
{
    res.normals = false;
    meshes = { {new LBuffer(app, vertices, indices), new LImage(res, app->info.api)} };
    init();
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
    if (reflexCubeMap != UNINITIALIZED)
    {
        glDeleteTextures(1, &reflexCubeMap);
        glDeleteTextures(1, &depthMap);
    }
    if (reflexFBO != UNINITIALIZED)
        glDeleteFramebuffers(1, &reflexFBO);
}

void LGraphics::LModel::draw()
{
    auto shader = (LShaders::OpenGLShader*)app->modelShader.get();
    if (app->drawingInShadow)
    {
        shader = dynamic_cast<LPointLight*>(app->currentLight)
            ? (LShaders::OpenGLShader*)app->shadowCubeMapModelShader.get()
            : (LShaders::OpenGLShader*)app->shadowMapModelShader.get();
    }
    else if (app->drawingReflex)
        shader = ((LShaders::OpenGLShader*)app->reflexModelShader.get());
    else if (app->drawingPicking)
        shader = ((LShaders::OpenGLShader*)app->pickingModelShader.get());

    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);

    // тут нужна оптимизация
    model = calculateModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model_"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shaderProgram, "playAnimation"), playAnimation_);
    if (playAnimation_ && app->drawingInShadow)
        animator.UpdateAnimation(app->getDeltaTime());

    const auto& transforms = animator.GetFinalBoneMatrices();
    if (transforms.size() && !app->drawingPicking)
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "finalBonesTrans"), transforms.size(), GL_FALSE,
            glm::value_ptr(transforms[0]));
    if (app->drawingPicking)
        glUniform2i(glGetUniformLocation(shaderProgram, "ids"), id,L_MODEL);

    FOR(i, 0, meshes.size())
    {
        if (meshes[i].image)
        {
            if (!app->drawingInShadow && !app->drawingPicking)
            {
#ifdef MEGATEXTURE_LG
                auto& diffuse = meshes[i].image->getDiffuse();
                const auto castedDiff = TO_GL(diffuse);
                auto& normal = meshes[i].image->getNormal();
                const auto castedNorm = TO_GL(normal);
                auto& parallax = meshes[i].image->getParallax();
                const auto castedParallax = TO_GL(parallax);
                auto& reflex = meshes[i].image->getReflex();
                const auto castedReflex = TO_GL(reflex);

                glUniform2f(glGetUniformLocation(shaderProgram, "offset"), castedDiff.offset.x, castedDiff.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSize"), castedDiff.size.x, castedDiff.size.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "offsetNormal"), castedNorm.offset.x, castedNorm.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSizeNormal"), castedNorm.size.x, castedNorm.size.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "offsetParallax"), castedParallax.offset.x, castedParallax.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSizeParallax"), castedParallax.size.x, castedParallax.size.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "offsetReflex"), castedReflex.offset.x, castedReflex.offset.y);
                glUniform2f(glGetUniformLocation(shaderProgram, "textureSizeReflex"), castedReflex.size.x, castedReflex.size.y);
                glUniform1i(glGetUniformLocation(shaderProgram, "normalMapping"), getNormalMapping(i));
                glUniform1i(glGetUniformLocation(shaderProgram, "parallaxMapping"), getParallaxMapping(i));
                glUniform1i(glGetUniformLocation(shaderProgram, "reflexMapping"), getReflexMapping(i));

                glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "inverseModel"), 1, GL_FALSE,
                    glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
                if (app->currentLight)
                {
                    glActiveTexture(GL_TEXTURE1);
                    if (dynamic_cast<LPointLight*>(app->currentLight))
                        glBindTexture(GL_TEXTURE_CUBE_MAP, app->currentDepthMap);
                    else
                        glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
                }
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.idNormal);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.idParallax);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, app->megatexture.idReflex);
                if (!app->drawingReflex)
                {
                    glActiveTexture(GL_TEXTURE5);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, reflexCubeMap);
                }
#endif
            }
        }
        glBindVertexArray(meshes[i].buffer->getVaoNum());
        if (meshes[i].buffer->getIndices().size())
            glDrawElements(GL_TRIANGLES, meshes[i].buffer->getIndices().size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, meshes[i].buffer->getVertices().size());
        glBindVertexArray(0);
    }
}

bool LGraphics::LModel::getSpeedModifier() const
{
    return animator.speed;
}

void LGraphics::LModel::setSpeedModifier(float speed)
{
    animator.speed = speed;
}

void LGraphics::LModel::playAnimation()
{
    playAnimation_ = true;
}

void LGraphics::LModel::playAnimation(const std::string& name)
{
    animator.PlayAnimation(animations[name]);
}

void LGraphics::LModel::stopAnimation()
{
    playAnimation_ = false;
}

void LGraphics::LModel::restartAnimation()
{
    animator.m_CurrentTime = 0.0f;
}

size_t LGraphics::LModel::getReflexSize() const
{
    return reflexSize;
}

LGraphics::LModel::LModel(LApp* app, const std::string& path, bool cropTextureCoords,size_t dummy)
    :LShape(app)
{
    std::string pathStr(path);
    LResourceManager::loadModel(this, pathStr, cropTextureCoords);
}

void LGraphics::LModel::init()
{
    rotateX(180.0f);
    setShader(app->modelShader.get());
    app->toCreateM.push(this);
    if (animations.size())
        animator.PlayAnimation(animations.begin()->second);
    //reflexWidth = app->getWindowSize().x;
    //reflexHeight = app->getWindowSize().y;
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
