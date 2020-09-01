#pragma once

#include <functional>
#include <vector>

#include "vectors.h"
#include "Lshaders.h"
#include "LImage.h"

namespace LGraphics
{
    class LApp;

    /*!
    @brief Абстрактный класс виджета. От него наследуются все виджеты Lizard Graphics.
    */
    class LWidget : public LImage
    {
    public:

        /*void setApp(LApp* app_) { this->app = app_; }  ///< Устанавливает приложение (окно) виджета.*/

        virtual void tick() {}
        virtual void draw() = 0; ///< Рисует виджет.

        /*!
        @brief Устанавливает цвет виджету.
        @param val - float [0;1].
        */
        virtual void transparency(const float val) = 0;

        /*!
        @brief Устанавливает цвет виджету.
        @param r - красный цвет, [0;255].
        @param g - зелёный цвет, [0;255].
        @param b - синий цвет, [0;255].
        */
        virtual void color(const unsigned char r, const unsigned char g, const unsigned char b) = 0;

        /*!
        @brief Устанавливает цвет виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        virtual void color(const fvect3 val) = 0;

        /*!
        @brief Устанавливает размеры виджету.
        @param x - x координата.
        @param y - y координата.
        @param z - z координата.
        */
        virtual void scale(const float x, const float y, const float z) = 0;

        /*!
        @brief Устанавливает размеры виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        virtual void scale(const fvect3 val) = 0;


        /*!
        @brief Перемещает виджет.
        @param val - вектор.
        */
        virtual void move(const fvect3 val) = 0;

        /*!
        @brief Перемещает виджет.
        @param x - x координата на сцене.
        @param y - y координата на сцене.
        @param z - z координата на сцене.
        */
        virtual void move(const float x, const float y, const float z) = 0;

        /*!
        @brief Перемещает виджет.
        @param x - пиксель по x.
        @param y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        virtual void move(const size_t x, const size_t y) = 0;

        /*!
        @brief Перемещает виджет.
        @param val - вектор, где x - пиксель по x, y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        virtual void move(const szvect2 v) = 0;

        /*!
        @brief устанавливает функцию, которая будет срабатывать при наведении мышки на виджет.
        */
        void setMouseOnItEventFunction(std::function<void()> fun) { mouseOnItFunction = fun; }

        virtual void turnOffColor() = 0;

        void setAnimation(std::function<void()> fun);
        void doAnimation();

        void setBreakingAnimation(std::function<void()> fun);
        void breakAnimation();

        virtual float getTransparency() const = 0;   ///< Возвращает прозрачность виджета.
        virtual fvect3 getColor() const = 0;         ///< Возвращает цвет виджета.
        virtual fvect3 getScale() const = 0;         ///< Возвращает размеры виджета.
        virtual fvect3 getMove() const = 0;          ///< Возвращает вектор move виджета.
        virtual fvect3 getCenter() const = 0;        ///< Возвращает центр виджета.

        virtual void setShader(LShaders::Shader* shader) = 0;  ///< Устанавливает шейдер виджету.

        virtual bool mouseOnIt() = 0;  ///< Возвращает находится ли мышка на виджете.

        void show() { isHidden_ = false; }   ///< Показать виджет.
        void hide() { isHidden_ = true; }    ///< Скрыть виджет.
        void setHidden(bool hide) { isHidden_ = hide; }   ///< Установить видимость виджета.
        bool isHidden() const { return isHidden_; }       ///< Возвращает спрятан ли виджет.

        virtual void setLabelColor(unsigned char r, unsigned char g, unsigned char b) = 0; ///< Устанавливает цвет метке виджета.
        virtual void setLabel(const std::string label) = 0;  /// Устанавливает текст метке виджета.

        const std::vector<LWidget*>& getInnerWidgets() const { return innerWidgets; }

        virtual ~LWidget() {}

    protected:

        LApp* app = nullptr;    ///< Указатель на приложение.
        bool isHidden_ = false; ///< Видимость виджета.
        std::function<void()> mouseOnItFunction;  ///< Функция, срабатывающая при наведении мышки на виджет.
        std::function<void()> animation = std::function<void()>([](){});
        std::function<void()> breakingAnimation = std::function<void()>([]() {});

        std::vector<LWidget*> innerWidgets;

        virtual void alignLabel() = 0;
        virtual void updateLabelPos() = 0;

        //LIWidget()
        //    :LImage(nullptr){}

        //*!
        //@brief Конструктор.
        //@param image - Копия LImage.
        //*/
        //LIWidget(const LImage& image)
        //    :LImage(image){}

        /*!
        @brief Конструктор.
        @param path - Путь к изображению.
        */
        LWidget(LApp* app, const char* path, bool lazy = true);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
        LWidget(LApp* app, const unsigned char* bytes, size_t size, bool lazy = true);

        virtual void init(LApp* app, bool lazy);
    };
}
