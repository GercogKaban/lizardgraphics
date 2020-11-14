#pragma once

#include "LRectangleShape.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LWRectangle : public LRectangleShape
    {
    public:

        const char* getObjectType() override { return "LRectangleShape"; }
        LWRectangle(LApp* app, const char* path = nullptr);
        bool isInterfaceObject() const override { return false; }

        virtual glm::vec2 getScreenCoords() const;

        void setMatrices(LApp* app);
        void setMatrices();
        void draw() override;

    private:

        glm::mat4 calculateModelMatrix() const;
        glm::mat4 model;
        glm::mat4 projection;
    };

}

