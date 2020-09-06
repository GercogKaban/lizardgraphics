#pragma once

#include "LRectangleShape.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LWRectangle : public LRectangleShape
    {
    public:

        LWRectangle(LApp* app, const char* path = nullptr, bool lazy = true);
        void setMatrices(LApp* app);
        void setMatrices();
        void draw() override;

    private:

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection, view;
    };

}

