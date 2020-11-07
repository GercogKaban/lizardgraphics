#pragma once

#include <mutex>

#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "Lshaders.h"
#include "LIRectangle.h"

namespace LGraphics
{
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
        friend LRectangleShape;

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
        szvect2 getWindowSize() const { return szvect2(width, height); }

        /*!
        @brief Возвращает дескриптор GLFW окна.

        */
        GLFWwindow* getWindowHandler() { return window; }

        void addText(std::string text, fvect2 pos, float scale, fvect3 color);
        void popText();
        LWidget* getActiveWidget();

        void lockFps(size_t fps_) { fpsLock = fps_; }

        void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window, resolutionX, resolutionY); }
        void setActiveWidget(LWidget* w) { activeWidget = w; }
        void setMatrices(glm::mat4 view, glm::mat4 projection);
        void setWidgetsMovability(bool movability) { widgetsMovability = movability; }

        void refreshObjectMatrices();

        void setKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> callback);
        void setMouseCallback(std::function<void(GLFWwindow* w, int button, int action, int mods)> callback);
        void setScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)>callback);

        std::vector<LWidget*>* getObjects() { return &objects; }
        std::vector<Text>& getTextObjects() { return textObjects; }

        LShaders::Shader* getStandartWorldObjShader() const { return standartWorldObjShader; }
        LShaders::Shader* getStandartInterfaceShader() const { return standartInterfaceshader; }
        LShaders::Shader* getStandartCheckMarkShader() const { return checkMarkShader; }

        glm::mat4 getViewMatrix() const { return view; }
        glm::mat4 getProjectionMatrix() const { return projection; }

        //void addSizeToTexturesToInitVector(const size_t size);
        //void addTextureToInit(LWidget* widget) { texturesToInit.push_back(widget);}

        std::mutex& getOpenGlDrawing() { return openGlDrawing; }

        void setSleepTime(size_t milliseconds) { sleepTime = milliseconds;}
        size_t getSleepTime() const { return sleepTime; }

        //void setWindowedMode() { glfwSetWindowMonitor(window, NULL, 0, 0, width, height, 10000); }
            
    private:

        void setMatrices();
        void addObject(LWidget* w);
        void moveWidgetToMouse(LWidget* w);

        LLine* textRenderer;

        void init();
        void initLEngine();
        void initOpenGl();

        void initTextures();

        void checkEvents();

        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void character_callback(GLFWwindow* window, unsigned int codepoint);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        GLFWwindow* window;
        std::vector<LWidget*> objects;
        std::vector<Text> textObjects;

        int width, height;

        size_t fps = 0, prevFps = 0, fpsLock = SIZE_MAX;
        size_t sleepTime = 0;

        LWidget* activeWidget = nullptr;
        LWidget* prevActiveWidget = nullptr;
        LWidget* widgetToMove = nullptr;
        bool widgetsMovability = false;

        LBuffer* standartRectBuffer;
        LShaders::Shader* standartInterfaceshader, *standartWorldObjShader, *checkMarkShader;

        //std::vector<LWidget*> texturesToInit;

        glm::mat4 view, projection;

        //bool initingTextures = false;

        std::mutex openGlDrawing;

        std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {};
        std::function<void(GLFWwindow* w, int button, int action, int mods)> mouseCallback = [](GLFWwindow* w, int button, int action, int mods) {};
        std::function<void(GLFWwindow* window, double xoffset, double yoffset)> scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {};
    };
}

