#include "LModelBuffer.h"

void LGraphics::LModelBuffer::setBuffers()
{
    setVerts();
    setInds();
    genBuffers();
}

LGraphics::LModelBuffer::LModelBuffer(LApp* app, float* vertices, uint16_t* indices, size_t verticesCount, size_t indicesCount, size_t normalsCount)
{
    this->app = app;
    this->verticesCount = verticesCount;
    this->indicesCount = indicesCount;
    this->normalsCount = normalsCount;

    this->vertices = new float[verticesCount];
    memcpy(this->vertices, vertices, verticesCount * sizeof(decltype(vertices)));

    ebo = new uint16_t[indicesCount];
    memcpy(ebo, indices, indicesCount * sizeof(decltype(ebo)));

    genBuffers();
}