#pragma once

#include "LRectangleShape.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LWRectangle : public LRectangleShape
    {
    public:

        friend LApp;

        LWRectangle(LApp* app, const char* path = nullptr);
        const char* getObjectType() const override { return "LWRectangle"; }
        //bool isInterfaceObject() const override { return false; }

        virtual glm::vec4 getScreenCoords() const;

        void setMatrices(LApp* app);
        void setMatrices();

#ifdef VULKAN
        void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override;
#endif

#ifdef OPENGL
        void draw() override;
#endif
        glm::mat4 getModelMatrix() const { return calculateModelMatrix(); }

        int effectIndex = 0;

    protected:
 
        glm::mat4 calculateModelMatrix() const;
        glm::mat4 model;
        glm::mat4 projection;
    };

}

