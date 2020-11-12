#pragma once

#include "LRectangleShape.h"

namespace LGraphics
{
    class LColorBar : public LRectangleShape
    {
    public:
        const char* getObjectType() override { return "LColorBar"; }

        LColorBar(LApp* app);
        void draw() override;


        void setFirstColor(const unsigned char r, const unsigned char g, const unsigned char b);
        void setSecondColor(const unsigned char r, const unsigned char g, const unsigned char b);

        // [0.0f;1.0f]
        void setFullness(float fullness);

    protected:

        void color(const unsigned char r, const unsigned char g, const unsigned char b) override {}
        void color(const fvect3 val) override {}

        fvect3 firstColor, secondColor;
        size_t border = 0;

    };
}