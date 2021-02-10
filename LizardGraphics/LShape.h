#pragma once

#include "LWidget.h"
//#include "include/glm/gtc/matrix_transform.hpp"
//#include "vectors.h"

namespace LGraphics
{
    class LBuffer;
    /*!
    @brief Переопределяет абстрактный класс LWidgetI.
    */
    class LShape : public LWidget
    {
    public:

        const char* getObjectType() const override { return "LShape"; }
        //void setShader(LShaders::Shader* shader) override;  ///< Устанавливает шейдер виджету.
        //LShaders::Shader* getShader() { return shader; }

        /*!
        @brief Устанавливает цвет виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        void color(const glm::vec3 val) override;

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
        void scale(const glm::vec3 val) override;

        /*!
        @brief Устанавливает размеры виджету.
        @param x - x координата.
        @param y - y координата.
        @param z - z координата.
        */
        void scale(const float x, const float y, const float z) override;

        void scaleWithoutAlign(const glm::vec3 val);

        /*!
        @brief Перемещает виджет.
        @param val - вектор.
        */
        void move(const glm::vec3 val) override;

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
        void move(const glm::vec<2, size_t> val) override;

        float getTransparency() const override { return transparency_; } ///< Возвращает прозрачность виджета.
        glm::vec3 getColor() const override { return color_; } ///< Возвращает цвет виджета.
        glm::vec3& getScale() override { return scale_; } ///< Возвращает размеры виджета.
        glm::vec3& getMove() override;   ///< Возвращает расположение виджета.

        void turnOffColor() override;

        LShaders::Shader* getShader() const { return shader; }  ///< Возвращает шейдер.
           
        virtual ~LShape();
    protected:

        /*!
        @brief
        @param path - путь к изображению.
        @param component - указатель на LBaseComponent.
        */
        LShape(LApp* app, const char* path);

#ifdef OPENGL
        /*!
        @brief
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        @param component - указатель на LBaseComponent.
        */
        LShape(LApp* app, const unsigned char* bytes, size_t size);
#endif

        void setBuffer(LBuffer* buffer);  ///< Устанавливает буфер.

        glm::vec3 scale_ = glm::vec3(0.5f, 0.5f, 1.0f);  ///< Вектор размеров виджета.
        glm::vec3 move_ = glm::vec3(0.0f, 0.0f, 0.0f);   ///< Вектор расположение виджета.
        glm::vec3 color_ = glm::vec3(0.0f, 0.0f, 0.0f);  ///< Вектор цвета виджета.

//      glm::mat4 projection_ = glm::mat4(1.0f);

        //glm::mat4 rotate_ = glm::mat4(1.0f);

        float transparency_ = 1.0f;                ///< Прозрачность виджета.

        LBuffer* buffer = nullptr; ///< Координатный буфер (вершини, индексы, текстурные координаты).
    };
}

