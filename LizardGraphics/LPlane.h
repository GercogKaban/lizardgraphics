#pragma once

#include "LImagedShape.h"
#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LApp;
    class METAGEN_CLASS LPlane : public LImagedShape
    {
    public:

        friend LApp;

        LPlane(LApp* app, ImageResource res);
        const char* getObjectType() const override { return "LPlane"; }
        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override{}

        static void drawInstanced();

        GEN_METADATA void setModel(const glm::mat4& model) override;
        GEN_METADATA void scale(const float x, const float y, const float z) override;
        GEN_METADATA void move(const float x, const float y, const float z) override;

        GEN_METADATA void rotateX(float angle) override;
        GEN_METADATA void rotateY(float angle) override;
        GEN_METADATA void rotateZ(float angle) override;

        GEN_METADATA void setNormalMapping(bool normalMapping) override;
        GEN_METADATA void setParallaxMapping(bool parallaxMapping) override;
        //
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

