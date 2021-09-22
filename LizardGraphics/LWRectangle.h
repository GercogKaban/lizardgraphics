#pragma once

#include "LRectangleShape.h"
#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LWRectangle : public LRectangleShape
    {
    public:

        friend LApp;

        LWRectangle(LApp* app, const char* path = nullptr, bool notRectangle = false);
        const char* getObjectType() const override { return "LWRectangle"; }

        //virtual glm::vec4 getScreenCoords() const;

        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override;
        void draw() override;

        static void drawInstanced();

        void setModel(const glm::mat4& model) override;
        void scale(const float x, const float y, const float z) override;
        void move(const float x, const float y, const float z) override;
        void setDiffuse(const char* path) override;

        void rotateX(float angle) override;
        void rotateY(float angle) override;
        void rotateZ(float angle) override;

    protected:

        static void updateBuffer();
        static void updateUniforms(LWidget::RectangleUniforms* buffer, size_t id);
        static void initInstanceBuffer();
        static void resetInstanceBuffer();

        static void updateBufferParallel(LWidget::RectangleUniforms* buffer, std::vector<LWRectangle*>& changed, size_t begin, size_t end);
            
        static bool needToResetBuffer;
        static GLuint vbo;
        static std::vector<RectangleUniforms> uniforms;

        static std::vector<LWRectangle*> objChanged;
    };

}

