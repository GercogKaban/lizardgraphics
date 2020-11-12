﻿#pragma once

#include "LRectangleShape.h"

namespace LGraphics
{
    /*!
    @brief Класс кнопки. При нажатии на неё мышкой срабатывает привязанная к ней функция.
    @todo Сделать, чтобы кнопка срабатывала при отжатии, поменять название (убрать I)
    */
    class LIButton : public LRectangleShape
    {
    public:

        friend LApp;

        const char* getObjectType() override { return "LIButton"; }
        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param path - путь к текстуре.
        */
        LIButton(LApp* app, const char* path = nullptr);

        LIButton(LApp* app, const unsigned char* bytes, size_t bytesSize);
        /*!
        @brief устанавливает функцию, которая будет срабатывать при нажатии на кнопку

        Пример использования:
        @code
        auto obj = new LGraphics::LIRectangle(&app, nullptr, "image.png",&c);
        auto b = new LGraphics::LIButton(&app, nullptr, "image.png",&c);
        b->setClickEventFunction([&]() {obj->setHidden(!obj->isHidden()); }); // прячет/показывает четырёхугольник.
        @endcode
        */
        void setClickEventFunction(std::function<void()> fun);

        void tick() override;

    protected:

        std::function<void()> clickFunction = [](){}; ///< Функция, срабатывающая при нажатии на кнопку.
        void doClickEventFunction(); ///< Вызывает clickFunction.
    };
}
