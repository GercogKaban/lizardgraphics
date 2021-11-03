#pragma once

#include <cmath>
#include "LBuffer.h"

namespace LGraphics
{
    class LApp;
    /*!
    @brief Класс буффера четырёхугольника.
    */
    class LRectangleBuffer : public LBuffer
    {
    public:

        const char* getObjectType() const override { return "LRectangleBuffer"; }

        LRectangleBuffer(LApp* app);

    private:
        static const std::vector<Vertex> vert;
        static const std::vector<uint32_t> ind;
    };
}
