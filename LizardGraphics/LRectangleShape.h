#pragma once
#include "LShape.h"

namespace LGraphics
{
    class LApp;
    class LBuffer;
    class LBaseComponent;

    /*!
    @brief Класс четурёхугольной формы.
    */
    class LRectangleShape : public LShape
    {
    public:

        bool mouseOnIt() override; ///< Возвращает находится ли мышка на виджете.

        void draw() override; ///< Рисует виджет на сцене.

        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param path - путь к текстуре.
        @param component - указатель на LBaseComponent.
        */
        LRectangleShape(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);

        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        @param component - указатель на LBaseComponent.
        */
        LRectangleShape(LApp* app, LObject* parent = nullptr, const unsigned char* bytes = nullptr, size_t size = 0, LBaseComponent* component = nullptr);

        void setLabel(const std::string label) override;
        void alignLabel() override;

        float calculateWidgetLength();

        fvect3 getCenter() const override;

        fvect3 getTopLeftCorner() const;
        fvect3 getTopRightCorner() const;
        fvect3 getBottomLeftCorner() const;
        fvect3 getBottomRightCorner() const;

    protected:

        void updateLabelPos() override;
    };
}

