#pragma once
#include "LShape.h"
#include "LImage.h"

namespace LGraphics
{
    class LApp;

    /*!
    @brief Класс четурёхугольной формы.
    */
    class LRectangleShape : public LShape, public LImage
    {
    public:

        const char* getObjectType() const override { return "LRectanlgeShape"; }

        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum);
        void draw();
        LRectangleShape(LApp* app, ImageResource res);

        const LBuffer* getBuffer() const { return buffer; }
        void setBuffer(LBuffer* buffer) { this->buffer = buffer; }
    protected:

        void init(LApp* app);
        LBuffer* buffer = nullptr; ///< Координатный буфер (вершины, индексы, текстурные координаты, нормали).
    };
}

