#include "pch.h"
#include "LApp.h"
#include "LError.h"
#include "Lshaders.h"
#include "LIButton.h"
#include "LRectangleBuffer.h"

namespace LGraphics
{
    LApp::LApp()
    {
        init(1920, 1080);
    }

    void LApp::loop()
    {

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            checkEvents();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for (auto& o : objects)
                if (!o->isHidden())
                    o->draw();
            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }

    void LApp::addObject(LWidgetI * w)
    {
        objects.push_back(w);
    }

    void LApp::init(const int width, const int height)
    {
        this->width = width, this->height = height;
        initOpenGl();
        initLEngine();
    }

    void LApp::initLEngine()
    {
        LError::init();
        //LShaders::interfaceShader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
        //baseRectangleBuffer = new LRectangleBuffer();
        //baseRectangleBuffer = new LRectangleBuffer();
        //baseRectangleTextBuffer->init();

        //auto background = new LIRectangle();
        //background->setScale({ 2.0f,2.0f,1.0f });
        //background->bindTexture(notexture, notextureSize);
        //addObject(background);
    }

    void LApp::initOpenGl()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(width, height, "window", nullptr, nullptr);
        glfwMakeContextCurrent(window);


        LApp* myWindow = this;
        glfwSetWindowUserPointer(window, myWindow);

        auto func = [](GLFWwindow* w, int button, int action, int mods)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
        };

        glfwSetMouseButtonCallback(window, func);

        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        int width_, height_;
        glfwGetFramebufferSize(window, &width_, &height_);
        glViewport(0, 0, width_, height_);
    }

    void LApp::checkEvents()
    {
        //for (auto& o : objects)
        //{
        //    o.
        //}
    }

    void LApp::releaseResources()
    {
        for (auto& x : objects)
            delete x;
        LError::releaseResources();

        //delete baseRectangleBuffer;
        //delete LShaders::interfaceShader;
    }

    void LApp::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            for (auto& o : objects)
            {
                if (dynamic_cast<LIButton*>(o))
                    if (o->mouseOnIt())
                        dynamic_cast<LIButton*>(o)->doClickEventFunction();
            }
        }
    }
}