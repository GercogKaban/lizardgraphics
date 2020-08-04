﻿#include "pch.h"
#include "LApp.h"
#include "LError.h"
#include "Lshaders.h"
#include "LTextEdit.h"
#include "LRectangleBuffer.h"
#include "LTimer.h"

namespace LGraphics
{
    LApp::LApp()
    {
        init();
    }

    void LApp::loop()
    {
        static LTimer t([&]()
        {prevFps = fps; fps = 0; }, std::chrono::milliseconds(1000));
        t.start();
        while (!glfwWindowShouldClose(window))
        {
            fps++;
            glfwPollEvents();
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for (auto& o : objects)
                if (!o->isHidden())
                    o->draw();
            for (auto& t : textObjects)
                LLine::display(t.text, t.pos.x, t.pos.y, t.scale, t.color);
            LLine::display(std::to_string(prevFps), 50.0f, (float)getWindowSize().y - 50.0f, 1.5f, { 1.0f,0.0f,0.0f });
            glfwSwapBuffers(window);
            for (auto& o : objects)
                o->tick();
        }
        t.stop();
        glfwTerminate();
    }

    void LApp::addText(std::string text, fvect2 pos, float scale, fvect3 color)
    {
        textObjects.push_back({ text,pos,scale,0.0f,color });
    }

    LWidgetI * LApp::getActiveWidget()
    {
        return activeWidget;
    }

    void LApp::addObject(LWidgetI * w)
    {
        objects.push_back(w);
    }

    void LApp::init()
    {
        initOpenGl();
        initLEngine();
    }

    void LApp::initLEngine()
    {
        LError::init();
        addText("Lizard Graphics v. 0.2", { static_cast<float>(width) - 400.0f,50.0f }, 0.7, { 1,0.75,0.81 });
    }

    void LApp::initOpenGl()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = mode->width;
        height = mode->height;

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, "window", glfwGetPrimaryMonitor(), nullptr);
        glfwMakeContextCurrent(window);
        
        glfwSetWindowUserPointer(window, this);

        auto mouse = [](GLFWwindow* w, int button, int action, int mods)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
        };

        auto key = [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
        };

        auto charCallback = [](GLFWwindow* window, unsigned int codepoint)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(window))->character_callback(window, codepoint);
        };

        glfwSetMouseButtonCallback(window, mouse);
        glfwSetKeyCallback(window, key);
        glfwSetCharCallback(window, charCallback);

        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glfwSwapInterval(1);

        int width_, height_;
        glfwGetFramebufferSize(window, &width_, &height_);
        glViewport(0, 0, width_, height_);
        textRenderer = &LLine(this);
    }

    void LApp::checkEvents()
    {
    }

    void LApp::releaseResources()
    {
        for (auto& x : objects)
            delete x;
        LError::releaseResources();
    }

    void LApp::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            for (auto& o : objects)
            {
                if (o->mouseOnIt())
                    activeWidget = o;
                if (dynamic_cast<LIButton*>(o) && o->mouseOnIt())
                        ((LIButton*)o)->doClickEventFunction();

                for (auto& innerW : o->getInnerWidgets())
                    if (dynamic_cast<LScroller*>(innerW) && ((LScroller*)innerW)->mouseOnIt())
                       activeWidget = innerW;
            }
        }

        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            for (auto& o : objects)
            {
                for (auto& innerW : o->getInnerWidgets())
                    if (dynamic_cast<LScroller*>(innerW) && activeWidget == (LScroller*)innerW)
                        activeWidget = ((LScroller*)innerW)->parent;
            }
        }
    }
    void LApp::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
    {
        LTextEdit* textEdit = dynamic_cast<LTextEdit*>(activeWidget);
        if (textEdit && action)
        {
            if (key == GLFW_KEY_BACKSPACE)
                textEdit->removeLastSymbol();
            else if (key == GLFW_KEY_ENTER)
                textEdit->addText(std::string(1, '\n'));
        }
    }

    void LApp::character_callback(GLFWwindow* window, unsigned int codepoint)
    {
        LTextEdit* textEdit = dynamic_cast<LTextEdit*>(activeWidget);
        if (textEdit)
            textEdit->addText(std::string(1, codepoint));
    }

}