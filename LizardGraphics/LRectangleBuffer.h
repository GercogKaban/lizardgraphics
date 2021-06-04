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
    protected:

        void setBuffers() override; ///< Инициализирует массив вершин и индексов.
        void setVerts() override; ///< Инициализирует массив вершин.
        void setInds() override; ///< Инициализирует массив индексов.

    public:

        const char* getObjectType() const override { return "LRectangleBuffer"; }

        LRectangleBuffer(LApp* app);

        glm::vec3 getTopLeftCorner() { return {vertices[9],vertices[10],vertices[11] };}
        glm::vec3 getTopRightCorner() { return { vertices[0],vertices[1],vertices[2] }; }
        glm::vec3 getBottomLeftCorner() { return { vertices[6],vertices[7],vertices[8] };}
        glm::vec3 getBottomRightCorner() { return { vertices[3],vertices[4],vertices[5] };}
    };
}

