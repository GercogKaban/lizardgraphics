#pragma once

#include <mutex>

#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "Lshaders.h"
#include "LIRectangle.h"
#include "ObjectPool.h"

#include "LWRectangle.h"
//#include "LMultiWRectangle.h"

namespace LGraphics
{
    class LTextRender;
    class LColorBar;
    namespace
    {
    }

    class LNonWidget;
    class Text;

    /*!
    @brief Класс приложения Lizard Graphics

    Объект этого класса должен объявляться перед любым из элементом 
    LGraphics, т.к. LApp инициализурент OpenGL, GLFW окно и некоторые 
    другие вспомогательные классы
    */
    class LApp : public LObject
    {
        friend LRectangleShape;
        friend LColorBar;
        friend LTextRender;

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

        void addText(Text* text);
        //void addText(std::string text, fvect2 pos, float scale, fvect3 color, int refreshingMode);
        Text* popText(int refreshingMode);
        LWidget* getActiveWidget();

        void lockFps(size_t fps_) { fpsLock = fps_; }

        void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window, resolutionX, resolutionY); }
        void setActiveWidget(LWidget* w) { activeWidget = w; }
        void setMatrices(glm::mat4 view, glm::mat4 projection);
        void setWidgetsMovability(bool movability) { widgetsMovability = movability; }

        void deleteWidget(LWidget* w);
        void removeWidget(LWidget* w);

        void refreshObjectMatrices();

        void setKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> callback);
        void setMouseCallback(std::function<void(GLFWwindow* w, int button, int action, int mods)> callback);
        void setScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)>callback);

        std::vector<LWidget*>& getInterfaceObjects(){ return interfaceObjects; }
        std::vector<LWidget*>& getNonInterfaceObjects() { return nonInterfaceObjects; }
        //std::vector<LWidget*>& getObjects() { return &objects; }
        //std::vector<Text>* getTextObjects() { return textObjects; }

        LShaders::Shader* getStandartWorldObjShader() const { return standartWorldObjShader; }
        LShaders::Shader* getStandartInterfaceShader() const { return standartInterfaceshader; }
        LShaders::Shader* getStandartCheckMarkShader() const { return checkMarkShader; }

        bool isPressed(int key);

        glm::mat4 getViewMatrix() const { return view; }
        glm::mat4 getProjectionMatrix() const { return projection; }

        std::mutex& getOpenGlDrawing() { return openGlDrawing; }

        void setSleepTime(size_t milliseconds) { sleepTime = milliseconds;}
        size_t getSleepTime() const { return sleepTime; }

        void switchScreenMode();

        void setBeforeDrawingFunc(std::function<void()> func) { beforeDrawingFunc = func; }
        void setAfterDrawingFunc(std::function<void()> func) { afterDrawingFunc = func; }

        void setViewPoint(glm::vec3 view) { viewPoint = view; /*refreshCamera();*/ };
        void setViewRadius(float radius) { viewRadius = radius; /*refreshCamera(); refreshProjection();*/ }

        glm::vec3 getViewPoint() const { return viewPoint; }
        float getViewRadius() const { return viewRadius; }

        LShaders::Shader* getLightningShader() { return experimentalLightShader; }

        unsigned int getDepthMap() const { return depthMap; }
         
        glm::vec3 getLightPos() const { return lightPos; }
        glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

        bool lightIsInited() const { return lightIsInited_; }
        void initLight() { lightIsInited_ = true; }

        fvect2 getMouseCoords() const { return mouseCoords; }

        ObjectPool<LWRectangle*> lwRectPool;
        ObjectPool<LColorBar*> lColorBarPool;

        glm::vec3 viewAxonometricVector = glm::vec3(1/sqrt(3));
        bool perspectiveProjection = false;

        std::vector<LNonWidget*> customObjects;

    protected:

        fvect2 mouseCoords = fvect2(0.0f);

        bool lightIsInited_ = false;
        glm::vec3 lightPos;
        glm::mat4 lightSpaceMatrix;

        void setLightPos(glm::vec3 lightPos);
        void setLightSpaceMatrix();

        void initTextures(std::vector<LWidget*>& objects);
        void setMatrices();
        void addObject(LWidget* w, bool isInterfaceObj);
        void moveWidgetToMouse(LWidget* w);

        void refreshCamera();
        void refreshProjection();


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
        LTextRender* textRenderer;

        glm::vec3 viewPoint = glm::vec3(14.0f, 14.0f, 0.0f);
        float viewRadius = 10.0f;
        std::vector<LWidget*> interfaceObjects;
        std::vector<LWidget*> nonInterfaceObjects;

        //std::vector<LWidget*> objects;
        std::vector<Text*> textObjects;

        int width, height;

        size_t fps = 0, prevFps = 0, fpsLock = SIZE_MAX, sleepTime = 0;

        LWidget* activeWidget = nullptr;
        //LWidget* prevActiveWidget = nullptr;
        LWidget* widgetToMove = nullptr;
        bool widgetsMovability = false;

        LBuffer* standartRectBuffer;

        LShaders::Shader* standartInterfaceshader, *standartWorldObjShader, *checkMarkShader, *colorBarShader, *experimentalLightShader,
            *shadowMap,*defaultShader, *multi_shadowMap, *multi_defaultShader;

        glm::mat4 view, projection;

        std::mutex openGlDrawing;
        std::map<int, bool> pressedKeys;

        bool fullscreen = false;

        std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {};
        std::function<void(GLFWwindow* w, int button, int action, int mods)> mouseCallback = [](GLFWwindow* w, int button, int action, int mods) {};
        std::function<void(GLFWwindow* window, double xoffset, double yoffset)> scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {};

        std::function<void()> beforeDrawingFunc = []() {};
        std::function<void()> afterDrawingFunc = []() {};

        unsigned int shadowWidth = 1024, shadowHeight = 1024;

        unsigned int depthMapFBO, depthMap;
        fvect4 borderColor = fvect4(1.0, 1.0, 1.0, 1.0);

        //bool refreshStaticText = false;
    };
}

