﻿#pragma once

#include <cmath>
#include "LBuffer.h"
//#include "vectors.h"

namespace LGraphics
{
    /*!
    @brief Класс буффера четырёхугольника.
    */
    class LMultiRectangleBuffer : public LBuffer
    {
    protected:

        void setBuffers() override; ///< Инициализирует массив вершин и индексов.
        void setVerts() override; ///< Инициализирует массив вершин.
        void setInds() override; ///< Инициализирует массив индексов.

        size_t rectCount;
        uint32_t* objIndex = nullptr;

    public:

        const char* getObjectType() const override { return "LMultiRectangleBuffer"; }
        LMultiRectangleBuffer(size_t rectCount);

        virtual void genBuffers() override;

        //fvect3 getTopLeftCorner() { return {vertices[9],vertices[10],vertices[11] };}
        //fvect3 getTopRightCorner() { return { vertices[0],vertices[1],vertices[2] }; }
        //fvect3 getBottomLeftCorner() { return { vertices[6],vertices[7],vertices[8] };}
        //fvect3 getBottomRightCorner() { return { vertices[3],vertices[4],vertices[5] };}
    };
}

