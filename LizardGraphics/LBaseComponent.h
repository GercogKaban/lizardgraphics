#pragma once
#include "Lshaders.h"
#include "LRectangleBuffer.h"

namespace LGraphics
{
    /*!
    @brief Компонент необходимы для конструкции виджетов Lizard Graphics.

    Сожержит в себе шейдер и буфер вершин, необходимых для рисования объектов.
    Должен передаваться 4-м параметром в конструктор LWidget.

    Пример:
    @code
    LGraphics::LBaseComponent c;

    auto obj = new LGraphics::LIRectangle(&app, nullptr, "image.png",&c);
    auto b = new LGraphics::LIButton(&app, nullptr, "image.png",&c);
    @endcode
    @todo В след. версиях необходимо инкапсулировать его в сами виджеты и
    не передавать его в конструктор.
    */
    class LBaseComponent : public LObject
    {
        LShaders::Shader* interfaceShader;
        LBuffer* baseRectangleBuffer;

        /*!
        @brief Конструктор
        */
    public:
        LBaseComponent()
        {
            interfaceShader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
            baseRectangleBuffer = new LRectangleBuffer();
            baseRectangleBuffer->init();
        }

        /*!
        @brief Деструктор
        */
        ~LBaseComponent()
        {
            delete interfaceShader;
            delete baseRectangleBuffer;
        }

        //void setRectangleBuffer(LBuffer* buffer) { baseRectangleBuffer = buffer; }
        //void setShader(LShaders::Shader* shader) { interfaceShader = shader; }

        /*!
        @brief Возвращает указатель на буфер вершин компонента.
        */

        LBuffer* getBuffer() { return baseRectangleBuffer; }
        /*!
        @brief Возвращает указатель на шейдер компонента.
        */
        LShaders::Shader* getShader() { return interfaceShader; }
    };
}
