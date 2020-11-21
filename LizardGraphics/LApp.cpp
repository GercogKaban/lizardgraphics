#include "pch.h"
#include "LApp.h"
#include "LError.h"
#include "LTextEdit.h"
#include "LRectangleBuffer.h"
#include "LWRectangle.h"
#include "LTimer.h"
#include "LCounter.h"
#include "LMultiWRectangle.h"
#include "LColorBar.h"

namespace LGraphics
{
    LApp::LApp()
    {
        init();
    }

    void LApp::loop()
    {
        LTimer t([&]()
            {prevFps = fps; fps = 0; }, std::chrono::milliseconds(1000));
        t.start();

        while (!glfwWindowShouldClose(window))
        {
            beforeDrawingFunc();

#if LG_MULTITHREAD
            openGlDrawing.lock();
#endif
            setMatrices();

            if (!lightIsInited())
            {
                setLightPos(glm::vec3(4.0f, 2.0f, 3.0f));
                setLightSpaceMatrix();
                initLight();
            }

            fps++;
            initTextures();
            glfwPollEvents();
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // рисуем в карту теней
            glViewport(0, 0, shadowHeight, shadowWidth);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            for (auto& o : nonInterfaceObjects)
                if (!o->isHidden())
                {
                    o->setShader(shadowMap);
                    o->draw();
                }
            for (auto& o : customObjects)
                if (!o->isInterface)
            {
                o->shader = shadowMap;
                o->draw();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, getWindowSize().x, getWindowSize().y);

            // рисуем сцену
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glActiveTexture(GL_TEXTURE0);

            for (auto& o : nonInterfaceObjects)
                if (!o->isHidden())
                {
                    o->setShader(defaultShader);
                    o->draw();
                }
            for (auto& o : customObjects)
                if (!o->isInterface)
            {
                o->shader = defaultShader;
                o->draw();
            }

            glDisable(GL_DEPTH_TEST);
            LDISPLAY();
            for (auto& o : interfaceObjects)
                if (!o->isHidden())
                    o->draw();

            for (auto& o : customObjects)
                if (o->isInterface)
                    o->draw();


            LTextRender::display(std::to_string(prevFps), { 50.0f, (float)getWindowSize().y - 50.0f }, 1.5f, { 1.0f,0.0f,0.0f }, getWindowSize());
            
            //for (auto& t : textObjects)
                //LLine::display(t.text, t.pos.x, t.pos.y, t.scale, t.color);
            
            for (auto& o : interfaceObjects)
                o->tick();
            for (auto& o : nonInterfaceObjects)
                o->tick();

            afterDrawingFunc();
            textRenderer->displayText();
            glfwSwapBuffers(window);

#if LG_MULTITHREAD
            openGlDrawing.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
#endif
        }

        t.stop();
        glfwTerminate();
    }

    void LApp::addText(Text* text)
    {
        textRenderer->lastTextureNum++;
        textObjects.push_back(text);
    }

    Text* LApp::popText(int refreshingMode)
    {
        auto res = textObjects.back();
        textObjects.pop_back();
        return res;
    }

    LWidget* LApp::getActiveWidget()
    {
        return activeWidget;
    }

    void LApp::setMatrices(glm::mat4 view, glm::mat4 projection)
    {
        this->view = view;
        this->projection = projection;
    }

    void LApp::refreshObjectMatrices()
    {
        for (auto& obj : nonInterfaceObjects)
            if (dynamic_cast<LWRectangle*>(obj))
                dynamic_cast<LWRectangle*>(obj)->setMatrices(this);
    }

    void LApp::setKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> callback)
    {
        keyCallback = callback;
    }

    void LApp::setMouseCallback(std::function<void(GLFWwindow* w, int button, int action, int mods)> callback)
    {
        mouseCallback = callback;
    }

    void LApp::setScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)> callback)
    {
        scrollCallback = callback;
    }

    void LApp::deleteWidget(LWidget* w)
    {
        std::vector<LWidget*>& obj = w->isInterfaceObject() ? interfaceObjects : nonInterfaceObjects;
        for (size_t i = 0; i < obj.size(); ++i)
            if (obj.operator[](i) == w)
            {
                if (w->innerWidgets)
                for (size_t j = 0; j < w->innerWidgets->size(); ++j)
                    deleteWidget(w->innerWidgets->operator[](j));
                if (activeWidget == w) activeWidget = nullptr;
                if (widgetToMove == w) widgetToMove = nullptr;
                obj.erase(obj.begin() + i);
            }
        delete w;
    }

    void LApp::removeWidget(LWidget * w)
    {
        std::vector<LWidget*>& obj = w->isInterfaceObject() ? interfaceObjects : nonInterfaceObjects;
        for (size_t i = 0; i < obj.size(); ++i)
            if (obj.operator[](i) == w)
            {
                if (activeWidget == w) activeWidget = nullptr;
                if (widgetToMove == w) widgetToMove = nullptr;
                obj.erase(obj.begin() + i);
            }
    }

    bool LApp::isPressed(int key)
    {
        return pressedKeys[key];
    }

    void LApp::switchScreenMode()
    {
        if (!fullscreen)
        {
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height, 10000);
            fullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(window, NULL, 0, 0, width, height, 10000);
            fullscreen = false;
        }
    }

    void LApp::setLightPos(glm::vec3 lightPos)
    {
        this->lightPos = lightPos;
    }

    void LApp::setLightSpaceMatrix()
    {
        const float near_plane = 0.1f, far_plane = 35.0f;
        const float d = 12.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f)/*!!!*/, glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
    }

    void LApp::initTextures(std::vector<LWidget*>& objects)
    {
        for (auto& w : objects)
            if (!w->isInited())
            {
                w->init();
                if (w->innerWidgets)
                    initTextures(*w->innerWidgets);
            }
    }

    void LApp::setMatrices()
    {
        refreshCamera();
        refreshProjection();
    }

    void LApp::addObject(LWidget* w, bool isInterfaceObj)
    {
        if (isInterfaceObj)
            interfaceObjects.push_back(w);
        else
            nonInterfaceObjects.push_back(w);
    }

    void LApp::moveWidgetToMouse(LWidget* w)
    {
        if (!w || !widgetsMovability || !w->getWidgetMovability()) return;
        double mouse_x, mouse_y;
        glfwGetCursorPos(getWindowHandler(), &mouse_x, &mouse_y);
        auto mouse = pointOnScreenToGlCoords(getWindowSize(), { (float)mouse_x ,(float)mouse_y });
        w->move(fvect3{ (float)mouse.x,(float)mouse.y,w->getMove().z });
    }

    //void LApp::drawText()
    //{
    //    for (size_t i = 0; i < 3; ++i)
    //        LTextRender::displayText(textObjects[i], i);
    //}

    void LApp::refreshCamera()
    {
        //float t = sqrt(3);
        view = glm::lookAt(viewPoint + viewRadius * viewAxonometricVector,
            glm::vec3(viewPoint),
            glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void LApp::refreshProjection()
    {
        auto aspect = (float)getWindowSize().x / (float)getWindowSize().y;
        if (perspectiveProjection)
            projection = glm::perspective(45.0f, aspect, 0.01f, 100.0f);
        else projection = glm::ortho(viewRadius * -1.0f, viewRadius * 1.0f, viewRadius * -1.0f / aspect, viewRadius * 1.0f / aspect, -1.0f, 100.0f);
    }

    void LApp::init()
    {
        initOpenGl();
        initLEngine();
    }

    void LApp::initLEngine()
    {
        LError::init(this);
        standartRectBuffer = new LRectangleBuffer();
        standartInterfaceshader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
        standartWorldObjShader = new LShaders::Shader(LShaders::world_v, LShaders::interface_f);
        checkMarkShader = new LShaders::Shader(LShaders::interface_v, LShaders::checkMark_f);
        colorBarShader = new LShaders::Shader(LShaders::interface_v, LShaders::colorBar_f);
        experimentalLightShader = new LShaders::Shader("light_v.vs", "light_f.fs", false);
        shadowMap = new LShaders::Shader("shadowMap.vs", "shadowMap.fs", false);
        defaultShader = new LShaders::Shader("shadows.vs", "shadows.fs", false);
        //multi_shadowMap = new LShaders::Shader("multi_shadowMap.vs", "multi_shadowMap.fs", false);
        //multi_defaultShader = new LShaders::Shader("multi_shadows.vs", "multi_shadows.fs", false);
        textRenderer = new LTextRender(this);

        lwRectPool.setCreationCallback([&]()
        {
            auto lwRect = new LWRectangle(this);
            removeWidget(lwRect);
            //getNonInterfaceObjects().pop_back();
            return lwRect;
        });

        lwRectPool.setReleaseFunction([&]()
        {
            //while (auto w = lwRectPool.pop())
            //    deleteWidget(w);
        });

        lColorBarPool.setCreationCallback([&]()
            {
                auto lBar = new LColorBar(this);
                removeWidget(lBar);
                //getNonInterfaceObjects().pop_back();
                return lBar;
            });

        lColorBarPool.setReleaseFunction([&]()
            {
                //while (auto w = lwRectPool.pop())
                //    deleteWidget(w);
            });

        setMatrices();
        addText(new Text("Lizard Graphics v. 0.2", { static_cast<float>(width) - 400.0f,50.0f }, 0.7, { 1,0.75,0.81 }));

        //const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        //unsigned int depthMapFBO;
        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        //unsigned int depthMap;
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor_[] = { borderColor.x,borderColor.y,borderColor.z,borderColor.w };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor_);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void LApp::initOpenGl()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_DECORATED, GL_FALSE);


        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

#ifndef NDEBUG
        window = glfwCreateWindow(mode->width - 1, mode->height, "My Title", NULL, NULL);
        width = mode->width - 1;
        height = mode->height;
#else
        width = mode->width;
        height = mode->height;
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, "window", glfwGetPrimaryMonitor(), nullptr);
#endif

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        auto cursor_position = [](GLFWwindow* w, double xpos, double ypos)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(w))->cursor_position_callback(w, xpos, ypos);
        };

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

        auto scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset)
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
        };

        glfwSetCursorPosCallback(window, cursor_position);
        glfwSetMouseButtonCallback(window, mouse);
        glfwSetKeyCallback(window, key);
        glfwSetCharCallback(window, charCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwGetFramebufferSize(window, &width, &height);

        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glfwSwapInterval(0);

        int width_, height_;
        glfwGetFramebufferSize(window, &width_, &height_);
        glViewport(0, 0, width_, height_);
    }

    void LApp::checkEvents()
    {
    }

    void LApp::releaseResources()
    {
        for (auto& x : interfaceObjects)
            deleteWidget(x);
        for (auto& x : nonInterfaceObjects)
            deleteWidget(x);
        delete standartRectBuffer;
        delete standartInterfaceshader;
        delete standartWorldObjShader;
        delete checkMarkShader;
        LError::releaseResources();
    }

    void LApp::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        mouseCallback(window, button, action, mods);
        //bool out = false;
        auto objects = interfaceObjects;

        if (!objects.size()) return;

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            size_t num = 0;
            for (int i = objects.size() - 1; i > -1; --i)
                //for (auto& o = objects.rbegin(); o < objects.rend(); o++, num++)
            {
                auto o = objects[i];

                // тут нужна рекурсия
                if (o->getInnerWidgetsPtr())
                {
                    for (auto& innerW : (o)->getInnerWidgets())
                    {
                        if (dynamic_cast<LScroller*>(innerW) && ((LScroller*)innerW)->mouseOnIt())
                        {
                            if (activeWidget) activeWidget->breakAnimation();
                            activeWidget = innerW;
                            return;
                            //widgetToMove = innerW;
                            //((LScroller*)innerW)->moveScrollerToMouse();
                        }
                        else if (dynamic_cast<LIButton*>(innerW) && ((LIButton*)innerW)->mouseOnIt())
                        {
                            ((LIButton*)innerW)->doClickEventFunction();
                            return;
                        }
                    }
                }
               
                if (o->mouseOnIt())
                {
                    if (activeWidget) activeWidget->breakAnimation();
                    //prevActiveWidget = activeWidget;
                    activeWidget = o;
                    widgetToMove = activeWidget;

                    if (dynamic_cast<LIButton*>(o))
                    {
                        ((LIButton*)o)->doClickEventFunction();
                        return;
                    }
                }
            }
        }

        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            widgetToMove = nullptr;
            activeWidget = nullptr;
            //    for (auto& o : objects)
            //    {
            //        for (auto& innerW : o->getInnerWidgets())
            //            if (dynamic_cast<LScroller*>(innerW) && activeWidget == (LScroller*)innerW)
            //            {
            //                if (activeWidget) activeWidget->breakAnimation();
            //                activeWidget = ((LScroller*)innerW)->parent;
            //            }
            //    }
        }

        if (widgetToMove) moveWidgetToMouse(widgetToMove);
    }

    void LApp::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        mouseCoords = { (float)xpos,(float)ypos };
        if (widgetToMove) moveWidgetToMouse(widgetToMove);
    }

    void LApp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        pressedKeys[key] = action != GLFW_RELEASE;
        keyCallback(window, key, scancode, action, mods);
        LTextEdit* textEdit = dynamic_cast<LTextEdit*>(activeWidget);
        if (textEdit && action)
        {
            if (key == GLFW_KEY_BACKSPACE)
                textEdit->removeLastSymbol();
            else if (key == GLFW_KEY_ENTER)
                textEdit->addText(std::string(1, '\n'));
        }
        if (pressedKeys[GLFW_KEY_LEFT_ALT] && pressedKeys[GLFW_KEY_ENTER])
            switchScreenMode();
    }

    void LApp::character_callback(GLFWwindow* window, unsigned int codepoint)
    {
        LTextEdit* textEdit = dynamic_cast<LTextEdit*>(activeWidget);
        if (textEdit)
            textEdit->addText(std::string(1, codepoint));
    }

    void LApp::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        scrollCallback(window, xoffset, yoffset);
        LCounter* counter = dynamic_cast<LCounter*>(activeWidget);
        if (counter)
            counter->addNumber(yoffset);
    }

    void LApp::initTextures()
    {
        initTextures(interfaceObjects);
        initTextures(nonInterfaceObjects);
        //for (auto& w : interfaceObjects)
        //    if (w && !w->isInited())
        //    {
        //        w->init();
        //        for (auto& i : w->innerWidgets)
        //            i->init();
        //    }

        //for (auto& w : nonInterfaceObjects)
        //    if (w && !w->isInited())
        //        w->init();
    }

}