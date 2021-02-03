#include "LModel.h"
#include "LModelBuffer.h"
#include "LResourceManager.h"

LGraphics::LModel::LModel(LApp* app, const char* path, const char* texturePath, 
    const char* normalsPath, bool debugInfo)
    :LShape(app,nullptr), modelPath(path)
{
    // Base texture and normal map 
    //textures.resize(2);

    loadModel(path, debugInfo);

    if (texturePath)
        loadTexture(texturePath, BASE_TEXTURE);
    if (normalsPath)
        loadTexture(normalsPath, NORMALS);

    app->addObject(this, app->getModels());
#ifdef VULKAN
    shader = app->baseShader;
#endif
}

LGraphics::LModel::~LModel()
{
    delete buffer;
    if (meshesToDraw)
        delete[] meshesToDraw;
}

void LGraphics::LModel::loadTexture(const char* path, TextureType type)
{
    auto texture = LResourceManager::loadTexture(path);
    auto m = LResourceManager::models[modelPath]->textures[type] = texture;
    this->texture = texture;
    //textures = LResourceManager::models[modelPath]->textures;
}

void LGraphics::LModel::setMeshDrawing(size_t num, bool draw)
{
    meshesToDraw[num] = draw;
}

void LGraphics::LModel::loadModel(const char* modelPath, bool debugInfo)
{
    LResourceManager::loadModel(this, modelPath, debugInfo);
}

void LGraphics::LModel::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex, size_t objNum)
{
    VkBuffer vertexBuffers[] = { buffer->getVertBuffer() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    app->updateUniformBuffer(frameIndex, objNum, this);

    const uint32_t dynamicOffsets[] =
    { objNum * static_cast<uint32_t>(app->dynamicAlignment),
    };

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        getShader()->getPipelineLayout(), 0, 1, &app->descriptorSets[objNum * 2 + frameIndex], 1, dynamicOffsets);

    for (size_t mesh = 0; mesh < ((LModelBuffer*)buffer)->getIndBuffer().size(); ++mesh)
    {
        if (!meshesToDraw[mesh])
            continue;
        vkCmdBindIndexBuffer(commandBuffer, ((LModelBuffer*)buffer)->getIndBuffer()[mesh], 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(((LModelBuffer*)buffer)->indices[mesh].size()), 1, 0, 0, 0);
    }
}
