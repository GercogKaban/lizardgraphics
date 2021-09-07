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

        virtual glm::vec4 getScreenCoords() const;

        void setMatrices(LApp* app);
        void setMatrices();

        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override;
        void draw() override;

        static void drawInstanced();

    protected:
        static std::vector<RectangleUniforms> uniforms;

    //    glm::mat4 getModelMatrix() const { return calculateModelMatrix(); }

    //protected:
 
    //    glm::mat4 calculateModelMatrix() const;
    //    glm::mat4 model;
    //    glm::mat4 projection;
    };

}

