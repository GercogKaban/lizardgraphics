#pragma once

#include "LBuffer.h"

namespace LGraphics
{
    class LModel;

    class LModelBuffer : public LBuffer
    {
    public:

        friend LModel;

        void setBuffers() override;
        void setVerts() override;
        void setInds() override;

        void genBuffers() override;

        size_t getVertSize() const override;
        size_t getMeshSize(size_t num) const;

        LModelBuffer(LApp* app, std::vector<float> vertexBuf, 
            std::vector<std::vector<uint32_t>>);

        std::vector<VkBuffer>& getIndBuffer() { return indexBuffer; }

    private:

        std::vector<std::vector<uint32_t>> indices;
        std::vector<float> vertices;

        std::vector <VkBuffer> indexBuffer;
        std::vector <VkDeviceMemory> indexBufferMemory;
        VkBuffer texCoordsBuffer;
        VkDeviceMemory texCoordsMemory;
    };
}