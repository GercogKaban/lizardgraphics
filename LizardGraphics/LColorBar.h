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

        void move(const fvect3) override;
        void move(const float x, const float y, const float z) override;
        void move(size_t x, size_t y) override;

        void scale(const fvect3 scale) override;
        void scale(const float x, const float y, const float z) override;

        float getFullness() const { return fullness; }
        void setFirstColor(const unsigned char r, const unsigned char g, const unsigned char b);
        void setSecondColor(const unsigned char r, const unsigned char g, const unsigned char b);

        // [0.0f;1.0f]
        void setFullness(float fullness);

    protected:

        void color(const unsigned char r, const unsigned char g, const unsigned char b) override {}
        void color(const fvect3 val) override {}

        float fullness = 0.1f;
        fvect3 firstColor, secondColor;
        size_t border = 0;

    };
}