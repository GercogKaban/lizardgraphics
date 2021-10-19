#pragma once

#include "LRectangleShape.h"
#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LApp;
    class LPlane : public LRectangleShape
    {
    public:

        friend LApp;

        LPlane(LApp* app, ImageResource res);
        const char* getObjectType() const override { return "LWRectangle"; }

        //virtual glm::vec4 getScreenCoords() const;

        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override;
        void draw() override;

        static void drawInstanced();

        void setModel(const glm::mat4& model) override;
        void scale(const float x, const float y, const float z) override;
        void move(const float x, const float y, const float z) override;

        void rotateX(float angle) override;
        void rotateY(float angle) override;
        void rotateZ(float angle) override;

    protected:

        static void updateBuffer();
        static void updateUniforms(LWidget::PrimitiveUniforms* buffer, size_t id);
        static void initInstanceBuffer();
        static void resetInstanceBuffer();

        static void updateBufferParallel(LWidget::PrimitiveUniforms* buffer, std::vector<LPlane*>& changed, size_t begin, size_t end);
            
        static bool needToResetBuffer;
        static GLuint vbo;

        static std::vector<PrimitiveUniforms> uniforms;
        static std::vector<LPlane*> objChanged;
    };

}

