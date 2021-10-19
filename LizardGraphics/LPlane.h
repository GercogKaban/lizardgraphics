#pragma once

#include "LImagedShape.h"
#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LApp;
    class LPlane : public LImagedShape
    {
    public:

        friend LApp;

        LPlane(LApp* app, ImageResource res);
        const char* getObjectType() const override { return "LPlane"; }
        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override{}

        static void drawInstanced();

        void setModel(const glm::mat4& model) override;
        void scale(const float x, const float y, const float z) override;
        void move(const float x, const float y, const float z) override;

        void rotateX(float angle) override;
        void rotateY(float angle) override;
        void rotateZ(float angle) override;

    protected:

        static void updateBuffer();
        static void initInstanceBuffer();
        static void resetInstanceBuffer();
            
        static bool needToResetBuffer;
        static GLuint vbo;

        static std::vector<PrimitiveUniforms> uniforms;
        static std::vector<LShape*> objChanged;
    };

}

