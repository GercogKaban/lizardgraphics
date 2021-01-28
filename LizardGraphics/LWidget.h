#pragma once

#include <functional>
#include <vector>

#include "include/glm/glm.hpp"

//#include "vectors.h"
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

        friend LApp;
        /*void setApp(LApp* app_) { this->app = app_; }  ///< Устанавливает приложение (окно) виджета.*/

        const char* getObjectType() override { return "LWidget"; }
        virtual void tick() {}
#ifdef VULKAN
        virtual void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum) = 0; ///< Рисует виджет.

        enum UniformChanges
        {
            UNMODIFIED,
            ONE_BUFFER_TO_CHANGE,
            TWO_BUFFERS_TO_CHANGE,
            THREE_BUFFERS_TO_CHANGE,
        };
#endif
#ifdef OPENGL
        virtual void draw() = 0; ///< Рисует виджет.
#endif

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
        virtual void color(const glm::vec3 val) = 0;

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
        virtual void scale(const glm::vec3 val) = 0;


        /*!
        @brief Перемещает виджет.
        @param val - вектор.
        */
        virtual void move(const glm::vec3 val) = 0;

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
        virtual void move(const glm::vec<2, size_t> v) = 0;

        /*!
        @brief устанавливает функцию, которая будет срабатывать при наведении мышки на виджет.
        */
        void setMouseOnItEventFunction(std::function<void()> fun) { mouseOnItFunction = fun; }

        void rotateX(float angleDegree);
        void rotateY(float angleDegree);
        void rotateZ(float angleDegree);
        void setRotate(const glm::mat4& rotate);

        virtual void turnOffColor() = 0;

        virtual float getTransparency() const = 0;   ///< Возвращает прозрачность виджета.
        virtual glm::vec3 getColor() const = 0;         ///< Возвращает цвет виджета.
        virtual glm::vec3 getScale() const = 0;         ///< Возвращает размеры виджета.
        virtual glm::vec3 getMove() const = 0;          ///< Возвращает вектор move виджета.
        virtual glm::vec3 getCenter() const = 0;        ///< Возвращает центр виджета.
        virtual glm::mat4 getRotate() const { return rotate_; }

        LShaders::Shader* getShader() { return shader; }

        virtual bool mouseOnIt() = 0;  ///< Возвращает находится ли мышка на виджете.

        void show() { isHidden_ = false; }   ///< Показать виджет.
        void hide() { isHidden_ = true; }    ///< Скрыть виджет.
        void setHidden(bool hide) { isHidden_ = hide; }   ///< Установить видимость виджета.
        bool isHidden() const { return isHidden_; }       ///< Возвращает спрятан ли виджет.
        bool isInited() const { return isInited_; }

        virtual ~LWidget() {};

    protected:

        glm::mat4 rotate_ = glm::mat4(1.0f);

        LApp* app = nullptr;    ///< Указатель на приложение.
        bool isHidden_ = false; ///< Видимость виджета.
        std::function<void()> mouseOnItFunction;  ///< Функция, срабатывающая при наведении мышки на виджет.
        std::function<void()> animation = std::function<void()>([](){});
        std::function<void()> breakingAnimation = std::function<void()>([]() {});

        //bool drawOver_ = false;

        std::vector<LWidget*>* innerWidgets = nullptr;

        bool isInited_ = false;

        //bool widgetMovability = true;
        LShaders::Shader* shader = nullptr; ///< Шейдер.

        int changed = 3;
        //VkImageView newTexture = nullptr;

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
        LWidget(LApp* app, const char* path);

        /*!
        @brief Конструктор.
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        */
#ifdef OPENGL
        LWidget(LApp* app, const unsigned char* bytes, size_t size);
#endif

        //virtual void init(LApp* app);
        virtual void init();
    };
}
