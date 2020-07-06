﻿#pragma once

#include "LApp.h"

namespace LGraphics
{
    class LBaseComponent;

    /*!
    @brief Класс кнопки. При нажатии на неё мышкой срабатывает привязанная к ней функция.
    @todo Сделать, чтобы кнопка срабатывала при отжатии.
    */
    class LIButton : public LRectangleShape
    {
    public:

        friend LApp;

        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param path - путь к текстуре.
        @param component - указатель на LBaseComponent.
        */
        LIButton(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);

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

    protected:

        std::function<void()> clickFunction = [](){}; ///< Функция, срабатывающая при нажатии на кнопку.
        void doClickEventFunction(); ///< Вызывает clickFunction.
    };
}

