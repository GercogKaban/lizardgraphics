#pragma once

#include "LBuffer.h"

namespace LGraphics
{
    /*!
    @brief Класс буффера четырёхугольника.
    */
    class LRectangleBuffer : public LBuffer
    {
    protected:

        void setBuffers() override; ///< Инициализирует массив вершин и индексов.
        void setVerts() override; ///< Инициализирует массив вершин.
        void setInds() override; ///< Инициализирует массив индексов.
    };
}

