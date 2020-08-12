#pragma once

#include <vector>

#include "LLine.h"

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
        szvect2 getWindowSize() const { 
            return szvect2(width, height); 
        }

        /*!
        @brief Возвращает дескриптор GLFW окна.

        */
        GLFWwindow* getWindowHandler() { return window; }

        void addText(std::string text, fvect2 pos, float scale, fvect3 color);
        void popText();
        LWidgetI* getActiveWidget();

        void lockFps(size_t fps_) { fpsLock = fps_; }

        void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window, resolutionX, resolutionY); }
        void setActiveWidget(LWidgetI* w) { activeWidget = w; }

        std::vector<LWidgetI*>* getObjects() { return &objects; }
        std::vector<Text>& getTextObjects() { return textObjects; }
        //void setWindowedMode() { glfwSetWindowMonitor(window, NULL, 0, 0, width, height, 10000); }
            
    private:

        void addObject(LWidgetI* w);

        LLine* textRenderer;

        void init();
        void initLEngine();
        void initOpenGl();

        void checkEvents();

        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void character_callback(GLFWwindow* window, unsigned int codepoint);

        GLFWwindow* window;
        std::vector<LWidgetI*> objects;
        std::vector<Text> textObjects;

        size_t width, height;

        size_t fps = 0, prevFps = 0, fpsLock = SIZE_MAX;

        LWidgetI* activeWidget = nullptr;
    };
}

