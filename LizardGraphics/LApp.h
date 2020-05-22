#pragma once

#include <vector>

#include "LSymbol.h"

#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#include "Lshaders.h"
#include "LIRectangle.h"

#include "vectors.h"

namespace LGraphics
{
    class LWidgetI;
    class LBuffer;

    namespace
    {
    }

    /*!
    @brief Класс приложения Lizard Graphics

    Объект этого класса должен объявляться перед любым из элементом 
    LGraphics, т.к. LApp инициализурент OpenGL, GLFW окно и некоторые 
    другие вспомогательные классы
    */
    class LApp : public LObject
    {
        friend LIRectangle;
        friend LRectangleShape;
        friend LShape;

    public:
        
        LApp();
        ~LApp(){releaseResources();}

        /*!
        @brief Бесконечный цикл

        Открывает окно, при этом запуская бесконечный цикл, в котором
        рисуются все объекты сцены. Выход из цикла осуществляется при закрытии окна.
        */
        void loop();

        /*!
        @brief Возвращает размеры окна (в пикселях).

        */
        fvect2 getWindowSize() const { return { (float)width, (float)height }; }

        /*!
        @brief Возвращает дескриптор GLFW окна.

        */
        GLFWwindow* getWindowHandler() { return window; }

    private:

        void addObject(LWidgetI* w);

        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        //end = std::chrono::system_clock::now();

        void init(const int width, const int height);
        void initLEngine();
        void initOpenGl();

        void checkEvents();

        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        GLFWwindow* window;
        std::vector<LWidgetI*> objects;

        size_t width, height;

        //LBuffer* baseRectangleBuffer;

        //static LBuffer* baseRectangleTextBuffer;
    };
}

