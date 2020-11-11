﻿#pragma once
#include "LRectangleShape.h"

namespace LGraphics
{
    class LBaseComponent;

    /*!
    @brief Класс четырёхугольника.
    */
    class LIRectangle : public LRectangleShape
    {
    public:

        const char* getObjectType() override { return "LIRectangle"; }
        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param path - путь к текстуре.
        @param component - указатель на LBaseComponent.
        */
        LIRectangle(LApp* app, const char* path = nullptr);
        void tick() override;
    };
}
