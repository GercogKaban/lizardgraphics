#pragma once

#include "LBuffer.h"
#include "LWidgetI.h"
#include "LLine.h"

#include "vectors.h"

namespace LGraphics
{
    class LBaseComponent;

    /*!
    @brief Переопределяет абстрактный класс LWidgetI.
    */
    class LShape : public LWidgetI
    {
    public:

        void setShader(LShaders::Shader* shader) override;  ///< Устанавливает шейдер виджету.

        /*!
        @brief Устанавливает цвет виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        void color(const fvect3 val) override;

        /*!
        @brief Устанавливает цвет виджету.
        @param r - красный цвет, [0;255].
        @param g - зелёный цвет, [0;255].
        @param b - синий цвет, [0;255].
        */
        void color(const unsigned char r, const unsigned char g, const unsigned char b) override; 

        /*!
        @brief Устанавливает цвет виджету.
        @param val - float [0;1].
        */
        void transparency(const float val) override;

        /*!
        @brief Устанавливает размеры виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        void scale(const fvect3 val) override;

        /*!
        @brief Устанавливает размеры виджету.
        @param x - x координата.
        @param y - y координата.
        @param z - z координата.
        */
        void scale(const float x, const float y, const float z) override;

        void scaleWithoutAlign(const fvect3 val);

        /*!
        @brief Перемещает виджет.
        @param val - вектор.
        */
        void move(const fvect3 val) override;

        /*!
        @brief Перемещает виджет.
        @param x - x координата на сцене.
        @param y - y координата на сцене.
        @param z - z координата на сцене.
        */
        void move(const float x, const float y, const float z) override;

        /*!
        @brief Перемещает виджет.
        @param x - пиксель по x.
        @param y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        void move(const size_t x, const size_t y) override;

        /*!
        @brief Перемещает виджет.
        @param val - вектор, где x - пиксель по x, y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        void move(const szvect2 val) override;

        float getTransparency() const override { return transparency_; } ///< Возвращает прозрачность виджета.
        fvect3 getColor() const override { return color_; } ///< Возвращает цвет виджета.
        fvect3 getScale() const override { return scale_; } ///< Возвращает размеры виджета.
        fvect3 getMove() const override { return move_; }   ///< Возвращает расположение виджета.

        void turnOffColor() override;

        void setLabelColor(unsigned char r, unsigned char g, unsigned char b) override;
        void setLabel(const std::string label) override;
           
        virtual ~LShape()
        {
            //if (buffer)delete buffer;
        }

    protected:

        /*!
        @brief
        @param path - путь к изображению.
        @param component - указатель на LBaseComponent.
        */
        LShape(const char* path = nullptr, LBaseComponent* component = nullptr);

        /*!
        @brief
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        @param component - указатель на LBaseComponent.
        */
        LShape(const unsigned char* bytes, size_t size, LBaseComponent* component = nullptr);

        void setBuffer(LBuffer* buffer);  ///< Устанавливает буфер.

        const LShaders::Shader* getShader() const { return shader; }  ///< Возвращает шейдер.

        fvect3 scale_ = fvect3(0.5f, 0.5f, 1.0f);  ///< Вектор размеров виджета.
        fvect3 move_ = fvect3(0.0f, 0.0f, 0.0f);   ///< Вектор расположение виджета.
        fvect3 color_ = fvect3(0.0f, 0.0f, 0.0f);  ///< Вектор цвета виджета.

        Text label;
        float labelTextStartPosition = 0.0f;

        float transparency_ = 1.0f;                ///< Прозрачность виджета.

        LShaders::Shader* shader;                  ///< Шейдер.
        LBuffer* buffer;                           ///< Координатный буфер (вершини, индексы, текстурные координаты).
    };
}

