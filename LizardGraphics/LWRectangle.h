#pragma once

#include "LRectangleShape.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LWRectangle : public LRectangleShape
    {
    public:

        LWRectangle(LApp* app, const char* path = nullptr);
        void setMatrices(LApp* app);
        void setMatrices();
        void draw() override;

    private:

        glm::mat4 view, projection, model;
    };

}

