#include "LModelBuffer.h"

void LGraphics::LModelBuffer::setBuffers()
{
    setVerts();
    setInds();
    genBuffers();
}

void LGraphics::LModelBuffer::setVerts()
{
    //this->vertices = new float[verticesCount];
    //memcpy(vertices, vertices_, verticesCount * sizeof(decltype(vertices)));
}

void LGraphics::LModelBuffer::setInds()
{
    //ebo = new uint16_t[indicesCount];
    //memcpy(ebo, indices_, indicesCount * sizeof(decltype(ebo)));
}

void LGraphics::LModelBuffer::genBuffers()
{
#ifdef VULKAN
    indexBuffer.resize(indices.size());
    indexBufferMemory.resize(indices.size());
        
    app->createBuffer(vertices.data(), vertexBuffer, vertexBufferMemory,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getVertSize());
    for (size_t i = 0; i < indices.size(); ++i)
        app->createBuffer(indices[i].data(), indexBuffer[i], indexBufferMemory[i], 
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getMeshSize(i));

#endif
}

size_t LGraphics::LModelBuffer::getVertSize() const
{
    return verticesCount * sizeof(decltype(*vertices.data()));
}

size_t LGraphics::LModelBuffer::getMeshSize(size_t num) const
{
    return indices[num].size() * sizeof(decltype(*indices[num].data()));
}

LGraphics::LModelBuffer::LModelBuffer(LApp* app, std::vector<float> vertexBuf,
    std::vector<std::vector<uint32_t>> indices)
{
    this->app = app;
    this->vertices = vertexBuf;
    this->indices = indices;

    this->verticesCount = vertices.size();
    //this->indicesCount = indicesCount;
    //this->normalsCount = normalsCount;

    setBuffers();

    vertices.clear();
    indices.clear();
}

LGraphics::LModelBuffer::~LModelBuffer()
{
#ifdef VULKAN
    vmaDestroyBuffer(app->allocator, vertexBuffer, vertexBufferMemory);
    vertexBuffer = VK_NULL_HANDLE;

    for (size_t i = 0; i < indexBuffer.size(); ++i)
        vmaDestroyBuffer(app->allocator, indexBuffer[i], indexBufferMemory[i]);
#endif
}
