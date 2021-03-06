﻿#include "pch.h"

#define VMA_IMPLEMENTATION
#define VKB_VALIDATION_LAYERS
#include "LApp.h"
#include "LRectangleBuffer.h"
#include "LMultiWRectangle.h"
#include "LModel.h"
#include "LModelBuffer.h"
#ifdef WIN32
#include "CodeGen.h"
#include "../codegen.h"
#endif
#include "LResourceManager.h"
#include "GLFW/glfw3.h"
#include "LCubeBuffer.h"
#include "LSkyBox.h"
#include "LRectangleMirror.h"

namespace LGraphics
{
    LAppCreateInfo LApp::info;

    LApp::LApp(const LAppCreateInfo& info)
    {
        LOG_CALL
        initApp_(info);
        cubeInstantPool.setApp(this);
    }

    void LApp::initApp_(const LAppCreateInfo& info)
    {
#ifdef WIN32
        __try
        {
            this->info = info;
            if (!info.logFlags)
                this->info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;
            setupLG();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            LLogger::setFlags(info.logFlags);
            if (info.logFlags & ASYNC_LOG)
            {
                if (!LAsyncLogger::isStarted())
                    LAsyncLogger::start();
                handleSEH(exception_code());
                LAsyncLogger::emergencyStop();
            }
            else if (info.logFlags & SYNC_LOG)
            {
                handleSEH(exception_code());
                LSyncLogger::emergencyStop();
            }
            std::terminate();
        }
#else
        this->info = info;
        if (!info.logFlags)
            this->info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;
        setupLG();
#endif
    }

    LApp::~LApp()
    {
        releaseResources();
    }

    void LApp::drawScene()
    {
        cubeInstantPool.draw();
        //if (primitives.size())
        //    for (size_t i = 0; i < primitives.size(); ++i)
        //        primitives[i]->draw();
        if (!drawingInShadow)
        {
            glDepthFunc(GL_LEQUAL);
            if(skybox)
                skybox->draw();
            glDepthFunc(GL_LESS);
        }
    }

    void LApp::loop_()
    {
        LOG_CALL
        try
        {
            const size_t screenSize = info.wndWidth * info.wndHeight * sizeof(int);
            if (info.redactorMode)
            {
                glGenBuffers(1, &ssbo);
                objectsOnScreen = new int[screenSize / sizeof(int)];
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                glBufferData(GL_SHADER_STORAGE_BUFFER, screenSize, objectsOnScreen, GL_DYNAMIC_COPY);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
            }
            cubeInstantPool.initPool();
            //buff = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
            //cubeInstantPool.initPool();

            while (!glfwWindowShouldClose(window_))
            {
                refreshCamera();
                glfwPollEvents();

                if (info.logFlags & SYNC_LOG)
                    LSyncLogger::tick();

                while (toDelete.size())
                {
                    auto obj = toDelete.top();
                    if (obj.first->arrayIndex != getModels().size() + getPrimitives().size() - 1);
                    indexGaps.push_back(obj.first->arrayIndex);
                    if (obj.second == L_MODEL)
                        deleteWidget(obj.first, models);
                    else
                        deleteWidget(obj.first, primitives);
                    toDelete.pop();
                }

                while (toCreate.size())
                {
                    auto obj = toCreate.top();
                    if (obj.second == L_MODEL)
                        addObject(obj.first, models);
                    else if (obj.second == L_PRIMITIVE)
                        addObject(obj.first, primitives);
                    if (indexGaps.size())
                    {
                        obj.first->arrayIndex = indexGaps.front();
                        indexGaps.pop_front();
                    }
                    else
                        obj.first->arrayIndex = getModels().size() + getPrimitives().size() + 1;
                    toCreate.pop();
                }

                beforeDrawingFunc();

#if LG_MULTITHREAD
                openGlDrawing.lock();
#endif

                if (info.api == L_VULKAN)
                {
                    if (g_SwapChainRebuild)
                    {
                        int width, height;
                        glfwGetFramebufferSize(window_, &width, &height);
                        if (width > 0 && height > 0)
                        {
                            ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                            ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
                            g_MainWindowData.FrameIndex = 0;
                            g_SwapChainRebuild = false;
                        }
                    }

                    ImGui_ImplVulkan_NewFrame();
                }
                else if (info.api == L_OPENGL)
                    ImGui_ImplOpenGL3_NewFrame();

                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                imgui();
                ImGui::Render();
                if (info.api == L_VULKAN)
                {
                    // Rendering
                    ImDrawData* draw_data = ImGui::GetDrawData();
                    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
                    if (!is_minimized)
                    {
                        memcpy(&wd->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
                        FrameRender(wd, draw_data);
                        FramePresent(wd);
                    }
                }
                else if (info.api == L_OPENGL)
                {
                    if (info.redactorMode)
                        std::fill(objectsOnScreen, objectsOnScreen + screenSize / sizeof(int), -1);
                    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
                    setLightSpaceMatrix();
                    // рисуем в карту теней
                    glEnable(GL_DEPTH_TEST);
                    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    drawingInShadow = true;
                    glViewport(0, 0, shadowWidth, shadowHeight);

                    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    drawScene();
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    drawingInShadow = false;
                    glfwGetFramebufferSize(window_, (int*)&info.wndWidth, (int*)&info.wndHeight);
                    glViewport(0, 0, info.wndWidth, info.wndHeight);

                    // рисуем сцену
                    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    drawScene();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    if (info.redactorMode)
                    {
                        buff = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
                        memcpy(objectsOnScreen, buff, screenSize);
                        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
                    }
                }
                afterDrawingFunc();
                glfwSwapBuffers(window_);

#if LG_MULTITHREAD
                drawingMutex.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
#endif
            }
            if (info.redactorMode)
            {
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
                delete[] objectsOnScreen;
            }
            //glDeleteFramebuffers(1, &fbo);
            if (info.saveObjects == L_TRUE)
                CodeGen::generateCode("codegen.h", this, "app");
        }
        catch (std::exception& err)
        {
            handleCppException(err);
#ifdef WIN32
            if (info.saveObjects == L_TRUE)
                CodeGen::generateCode("codegen.h", this, "app");
#endif
            if (info.logFlags & ASYNC_LOG)
                LAsyncLogger::emergencyStop();
            else if (info.logFlags & SYNC_LOG)
                LSyncLogger::emergencyStop();
            std::terminate();
        }
    }

    void LApp::switchRendererTo(RenderingAPI api)
    {
        if (info.api == api)
            return;

        if (info.api == L_VULKAN)
        {
            releaseVulkanResources();
            releaseGlfwResources();
            info.api = api;
            initOpenGL();
        }
        else if (info.api == L_OPENGL)
        {
            releaseOpenGLResources();
            releaseGlfwResources();
            info.api = api;
            initVulkan();
        }
        setCursorEnabling(false);

        updateShaders();
        updateBuffers();
        updateTextures();

        for (auto& k : pressedKeys)
            k.second = false;
        afterSwitchingFunc();
    }

    LWidget* LApp::findByLID(int id)
    {
        for (auto& r : primitives)
            if (r->getId() == id)
                return r;
        return nullptr;
    }

    void LApp::loop()
    {
#ifdef WIN32
        __try
        {
            loop_();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (info.saveObjects == L_TRUE)
                CodeGen::generateCode("codegen.h", this, "app");
            emergencyStop(exception_code());
            std::terminate();
        }
#else
        loop_();
#endif

    }

    void LApp::updateShaders()
    {
        for (auto& p : primitives)
            p->setShader(getStandartShader());
        for (auto& m : models)
            m->setShader(getStandartShader());
    }

    void LApp::updateBuffers()
    {
        //for (auto& p : primitives)
        //    p->setBuffer(standartRectBuffer);
        //for (auto& m : models)
        //    m->setShader(getStandartShader());
    }

    void LApp::updateTextures()
    {
        //for (auto& r : rectangles)
        //{
        //    size_t dummy;
        //    r->setTexture((VkImageView)LResourceManager::loadTexture(r->getTexturePath().data(),dummy));
        //}
    }

    void LApp::emergencyStop(std::exception& exception)
    {
        if (info.logFlags & ASYNC_LOG)
        {
            handleCppException(exception);
            LAsyncLogger::emergencyStop();
        }
        else if (info.logFlags & SYNC_LOG)
        {
            handleCppException(exception);
            LSyncLogger::emergencyStop();
        }
        std::terminate();
    }

    void LApp::emergencyStop(unsigned long code)
    {
        if (info.logFlags & ASYNC_LOG)
        {
            handleSEH(code);
            LAsyncLogger::emergencyStop();
        }
        else if (info.logFlags & SYNC_LOG)
        {
            handleSEH(code);
            LSyncLogger::emergencyStop();
        }
        std::terminate();
    }

    void LApp::handleSEH(const size_t& code)
    {
#ifdef WIN32
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION:         LLogger::calls.push("\n\nerror: EXCEPTION_ACCESS_VIOLATION\ncalls history:\n"); break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    LLogger::calls.push("\n\nerror: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\ncalls history:\n"); break;
        case EXCEPTION_BREAKPOINT:               LLogger::calls.push("\n\nerror: EXCEPTION_BREAKPOINT\ncalls history:\n"); break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:    LLogger::calls.push("\n\nerror: EXCEPTION_DATATYPE_MISALIGNMENT\ncalls history:\n"); break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:     LLogger::calls.push("\n\nerror: EXCEPTION_FLT_DENORMAL_OPERAND\ncalls history:\n"); break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       LLogger::calls.push("\n\nerror: EXCEPTION_FLT_DIVIDE_BY_ZERO\ncalls history:\n"); break;
        case EXCEPTION_FLT_INEXACT_RESULT:       LLogger::calls.push("\n\nerror: EXCEPTION_FLT_INEXACT_RESULT\ncalls history:\n"); break;
        case EXCEPTION_FLT_INVALID_OPERATION:    LLogger::calls.push("\n\nerror: EXCEPTION_FLT_INVALID_OPERATION\ncalls history:\n"); break;
        case EXCEPTION_FLT_OVERFLOW:             LLogger::calls.push("\n\nerror: EXCEPTION_FLT_OVERFLOW\ncalls history:\n"); break;
        case EXCEPTION_FLT_STACK_CHECK:          LLogger::calls.push("\n\nerror: EXCEPTION_FLT_STACK_CHECK\ncalls history:\n"); break;
        case EXCEPTION_FLT_UNDERFLOW:            LLogger::calls.push("\n\nerror: EXCEPTION_FLT_UNDERFLOW\ncalls history:\n"); break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:      LLogger::calls.push("\n\nerror: EXCEPTION_ILLEGAL_INSTRUCTION\ncalls history:\n"); break;
        case EXCEPTION_IN_PAGE_ERROR:            LLogger::calls.push("\n\nerror: EXCEPTION_IN_PAGE_ERROR\ncalls history:\n"); break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       LLogger::calls.push("\n\nerror: EXCEPTION_INT_DIVIDE_BY_ZERO\ncalls history:\n"); break;
        case EXCEPTION_INT_OVERFLOW:             LLogger::calls.push("\n\nerror: EXCEPTION_INT_OVERFLOW\ncalls history:\n"); break;
        case EXCEPTION_INVALID_DISPOSITION:      LLogger::calls.push("\n\nerror: EXCEPTION_INVALID_DISPOSITION\ncalls history:\n"); break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: LLogger::calls.push("\n\nerror: EXCEPTION_NONCONTINUABLE_EXCEPTION\ncalls history:\n"); break;
        case EXCEPTION_PRIV_INSTRUCTION:         LLogger::calls.push("\n\nerror: EXCEPTION_PRIV_INSTRUCTION\ncalls history:\n"); break;
        case EXCEPTION_SINGLE_STEP:              LLogger::calls.push("\n\nerror: EXCEPTION_SINGLE_STEP\ncalls history:\n"); break;
        case EXCEPTION_STACK_OVERFLOW:           LLogger::calls.push("\n\nerror: EXCEPTION_STACK_OVERFLOW\ncalls history:\n"); break;
        default:  LLogger::calls.push("\n\nerror: UNKNOWN EXCEPTION\ncalls history:");
        }
#endif
    }

    void LApp::handleCppException(std::exception& err)
    {
#ifdef WIN32
        if (dynamic_cast<std::runtime_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_RUNTIME_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::logic_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_LOGIC_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::invalid_argument*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_INVALID_ARGUMENT: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::domain_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_DOMAIN_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::length_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_LENGTH_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::out_of_range*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_OUT_OF_RANGE: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_optional_access*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_OPTIONAL_ACCESS: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::range_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_RANGE_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::overflow_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_OVERFLOW_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::underflow_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_UNDERFLOW_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::system_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_SYSTEM_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::ios_base::failure*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_IOS_BASE_FAILURE: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::filesystem::filesystem_error*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_FILESYSTEM_ERROR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_typeid*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_TYPEID: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_cast*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_CAST: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_weak_ptr*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_WEAK_PTR: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_function_call*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_FUNCTION_CALL: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_alloc*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_ALLOC: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_array_new_length*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_ARRAY_NEW_LENGTH: ") + err.what() + "\ncalls history:");
        else if (dynamic_cast<std::bad_exception*>(&err))
            LLogger::calls.push(std::string("\n\nerror: EXCEPTION_BAD_EXCEPTION: ") + err.what() + "\ncalls history:");
        else
            LLogger::calls.push(std::string("\n\nerror: UNKNOWN_EXCEPTION: ") + err.what() + "\ncalls history:");
#endif
    }

    glm::vec<2, size_t> LApp::getWindowSize() const
    {
        return glm::vec<2, size_t>(info.wndWidth, info.wndHeight);
    }

    void LApp::setMatrices(glm::mat4 view, glm::mat4 projection)
    {
        this->view = view;
        this->projection = projection;
    }

    void LApp::refreshObjectMatrices()
    {
        //for (auto& obj : nonInterfaceObjects)
        //    if (dynamic_cast<LWRectangle*>(obj))
        //        dynamic_cast<LWRectangle*>(obj)->setMatrices(this);
    }

    LShaders::Shader* LApp::getStandartShader() const
    {
        return info.api == L_VULKAN? lightShader.get() : openGLLightShader.get();
    }

    bool LApp::isPressed(int key)
    {
        return pressedKeys[key];
    }

    void LApp::setCursorEnabling(bool enableCursor)
    {
        cursorEnabled = enableCursor;
        if (cursorEnabled)
        {
            prevCoords = getMouseCoords();
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            cursorModeSwitched = true;
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    void LApp::setUserMouseButtonCallback(std::function<void(GLFWwindow* window, int button, int action, int mods)> func)
    {
        LOG_CALL
        userMouseButtonCallback = func;
    }

    void LApp::setUserCursorCallback(std::function<void(GLFWwindow* window, double xpos, double ypos)> func)
    {
        LOG_CALL
        userCursorCallback = func;
    }

    void LApp::setUserKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> func)
    {
        LOG_CALL
        userKeyCallback = func;
    }

    void LApp::setUserCharacterCallback(std::function<void(GLFWwindow* window, unsigned int codepoint)> func)
    {
        LOG_CALL
        userCharacterCallback = func;
    }

    void LApp::setUserScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)> func)
    {
        LOG_CALL
        userScrollCallback = func;
    }

    void LApp::setLightPos(glm::vec3 lightPos)
    {
        LOG_CALL
        this->lightPos = lightPos;
    }

    void LApp::setLightSpaceMatrix()
    {
        LOG_CALL
        const float near_plane = 0.1f, far_plane = 35.0f;
        const float d = 12.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(7.5f,0.0f,7.5f)/*!!!*/, glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
    }

    void LApp::initTextures(std::vector<LWidget*>& objects)
    {
        LOG_CALL
        for (auto& w : objects)
            if (!w->isInited())
                w->init();
    }

    void LApp::setMatrices()
    {
        LOG_CALL
        refreshCamera();
        refreshProjection();
    }

    void LApp::setClearColor(glm::vec4 clearColor)
    {
        LOG_CALL
        clear_color.x = clearColor.x;
        clear_color.y = clearColor.y;
        clear_color.z = clearColor.z;
        clear_color.w = clearColor.w;
    }

    void LApp::refreshCamera()
    {
        LOG_CALL
        if (info.projection == L_PERSPECTIVE)
            view = glm::lookAt(
                cameraPos,
                cameraPos + cameraFront,
                cameraUp);
        else
            view = glm::lookAt(
                cameraPos + viewRadius * viewAxonometricVector,
                cameraPos + cameraFront,
                cameraUp);
    }

    void LApp::refreshProjection()
    {
        LOG_CALL
        const auto aspect = (float)getWindowSize().x / (float)getWindowSize().y;
        if (info.projection == L_PERSPECTIVE)
            projection = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);
        else projection = glm::ortho(viewRadius * -1.0f, viewRadius * 1.0f, viewRadius * -1.0f / aspect, viewRadius * 1.0f / aspect, -1.0f, 1000.0f);
    }

    void LApp::initErrorRecovering()
    {
        LLogger::setFlags(info.logFlags);
        if (info.logFlags & ASYNC_LOG)
        {
            if (!LAsyncLogger::isStarted())
                LAsyncLogger::start();
            LAsyncLogger::emergencyStop();
        }
        else if (info.logFlags & SYNC_LOG)
            LSyncLogger::emergencyStop();
    }

    void LApp::setupLG()
    {
        LOG_CALL

        try
        {
            initRenderer();
            initLEngine();
        }
        catch (std::exception& err)
        {
            handleCppException(err);
            initErrorRecovering();
            std::terminate();
        }
    }

    void LApp::initLEngine()
    {
        LOG_CALL
        LLogger::initErrors(info.logFlags);
        if (info.logFlags & ASYNC_LOG)
            LAsyncLogger::start();

        lwRectPool.setCreationCallback([&]()
        {
            auto lwRect = new LWRectangle(this);
            removeWidget(lwRect, primitives);
            return lwRect;
        });

        lwRectPool.setReleaseFunction([&]()
        {
            //while (auto w = lwRectPool.pop())
            //    deleteWidget(w);
        });

        setMatrices();
#ifdef WIN32
        if (info.loadObjects)
            genWidgets(this);
#endif
        //cubeInstantPool.initPool();
    }

#ifdef OPENGL
    void LApp::initRenderer()
    {
        glfwInit();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Window creation                                              //
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, GL_FALSE);

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifndef NDEBUG
        window_ = glfwCreateWindow(mode->width - 1, mode->height, "Lizard Graphics", NULL, NULL);
        width = mode->width - 1;
        height = mode->height;
#else
        width = mode->width;
        height = mode->height;

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
#endif
        window_ = glfwCreateWindow(mode->width, mode->height, "window", glfwGetPrimaryMonitor(), nullptr);

        glfwMakeContextCurrent(window_);

 //                                             Window creation                                              //
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        setWindowCallbacks();
        glfwGetFramebufferSize(window_, &width, &height);

        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glfwSwapInterval(0);


        int width_, height_;
        glfwGetFramebufferSize(window_, &width_, &height_);
        glViewport(0, 0, width_, height_);
    }
#endif

    void LApp::setWindowCallbacks()
    {
        LOG_CALL
        glfwSetWindowUserPointer(window_, this);

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

        glfwSetCursorPosCallback(window_, cursor_position);
        glfwSetMouseButtonCallback(window_, mouse);
        glfwSetKeyCallback(window_, key);
        glfwSetCharCallback(window_, charCallback);
        glfwSetScrollCallback(window_, scrollCallback);
    }

    void LApp::initOpenGL()
    {
        LOG_CALL
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            throw std::runtime_error("can't init glfw.");
        //info.poolSize = MAXSIZE_T;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //                                              Window creation                                              //
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (!info.wndHeight)
        {
            info.wndWidth = mode->width;
            info.wndHeight = mode->height;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, info.MSAA);
#ifndef NDEBUG
        window_ = glfwCreateWindow(info.wndWidth, info.wndHeight, "Lizard Graphics", NULL, NULL);
        //width = info.wndWidth;
        //height = info.wndHeight;
#else
        info.wndWidth = mode->width;
        info.wndHeight = mode->height;

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window_ = glfwCreateWindow(mode->width, mode->height, "Lizard Graphics", glfwGetPrimaryMonitor(), NULL);
#endif
        const char* glsl_version = "#version 330";

        glfwMakeContextCurrent(window_);

        setWindowCallbacks();
        glfwGetFramebufferSize(window_, (int*)(&info.wndWidth), (int*)(&info.wndHeight));

        glewExperimental = GL_TRUE;
        glewInit();
        if (!glewIsSupported("GL_ARB_bindless_texture"))
            throw std::runtime_error("can't load GL_ARB_bindless_texture!");

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (info.vsync == L_TRUE)
           glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        if (info.api == L_OPENGL)
        {
            openGLLightShader.reset(new LShaders::OpenGLShader(this, "shaders//shadows.vs", "shaders//shadows.fs"));
            //openGLLightShader.reset(new LShaders::OpenGLShader(this, "shaders//base.vs", "shaders//base.fs"));
            skyBoxShader.reset(new LShaders::OpenGLShader(this, "shaders//skybox.vs", "shaders//skybox.fs"));
            skyBoxMirrorShader.reset(new LShaders::OpenGLShader(this, "shaders//baseMirror.vs", "shaders//baseMirror.fs"));
            shadowMapShader.reset(new LShaders::OpenGLShader(this, "shaders//shadowMap.vs", "shaders//shadowMap.fs"));
        }
        standartRectBuffer = new LRectangleBuffer(this);
        standartSkyBoxBuffer = new LSkyBoxBuffer(this);
        standartCubeBuffer = new LCubeBuffer(this);
        glViewport(0, 0, info.wndWidth, info.wndHeight);

        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor_[] = { borderColor.x,borderColor.y,borderColor.z,borderColor.w };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor_);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void LApp::initVulkan()
    {
        LOG_CALL
        glfwSetErrorCallback(glfw_error_callback_Vk);
        if (!glfwInit())
            throw std::runtime_error("can't init glfw!");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (!info.wndHeight)
        {
            info.wndWidth = mode->width;
            info.wndHeight = mode->height;
        }

#ifdef NDEBUG
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window_ = glfwCreateWindow(info.wndWidth, info.wndHeight, "Lizard Graphics", glfwGetPrimaryMonitor(), NULL);
        fullscreen = true;
#else
        window_ = glfwCreateWindow(info.wndWidth, info.wndHeight, "Lizard Graphics", nullptr, NULL);
#endif

        //glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        setWindowCallbacks();
        // Setup Vulkan
        if (!glfwVulkanSupported())
            throw std::runtime_error("GLFW: Vulkan Not Supported\n");

        setupVulkan();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(window_, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.Allocator = g_Allocator;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, renderPass);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        // Upload Fonts
        {
            // Use any command queue
            VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
            VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

            auto err = vkResetCommandPool(g_Device, command_pool, 0);
            check_vk_result(err);
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(command_buffer, &begin_info);
            check_vk_result(err);

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &command_buffer;
            err = vkEndCommandBuffer(command_buffer);
            check_vk_result(err);
            err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
            check_vk_result(err);

            err = vkDeviceWaitIdle(g_Device);
            check_vk_result(err);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

        // Our statel
        IMGUI_CHECKVERSION();
    }

#ifdef VULKAN
    void LApp::initRenderer()
    {
        LOG_CALL
        if (info.api == L_OPENGL)
           initOpenGL();
        else if (info.api == L_VULKAN)
           initVulkan();
    }
#endif

    void LApp::checkEvents()
    {
        LOG_CALL
    }

    void LApp::releaseVulkanResources()
    {
        LOG_CALL
            auto imCount = wd->ImageCount;

        vkDeviceWaitIdle(g_Device);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();

        alignedFree(testStructV.model);

        for (size_t i = 0; i < imCount; ++i)
            vmaUnmapMemory(allocator, uniformBuffersMemory[i]);

        for (size_t i = 0; i < imCount; i++)
            vmaDestroyBuffer(allocator, uniformBuffers[i], uniformBuffersMemory[i]);

        vkDestroyDescriptorPool(g_Device, descriptorPool, nullptr);
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

        for (size_t i = 0; i < possibleMipLevels; ++i)
            vkDestroySampler(g_Device, textureSamplers[i], nullptr);

        auto resourseManager = LImage::resManager;

        for (auto it = resourseManager.textures.begin(); it != resourseManager.textures.end(); it++)
        {
            //vkDestroyImageView(g_Device, (VkImageView)std::get<0>(*(it->second)), nullptr);
            //vmaDestroyImage(allocator, std::get<1>(*(it->second)), std::get<2>(*(it->second)));

            //delete it->second;
        }

        resourseManager.textures.clear();

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(g_Device, swapChainImageViews[i], nullptr);
        }

        vkDestroyImageView(g_Device, depthImageView, nullptr);
        vmaDestroyImage(allocator, depthImage, depthImageMemory);

        vkDestroyRenderPass(g_Device, renderPass, nullptr);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(g_Device, framebuffer, nullptr);
        }

        vkDestroyDescriptorSetLayout(g_Device, descriptorSetLayout, nullptr);

        delete standartRectBuffer;

        //for (auto w : rectangles)
        //    delete w;
        //rectangles.clear();

        for (auto& m : models)
        {
            LModelBuffer* b = (LModelBuffer*)m->buffer;
            vmaDestroyBuffer(allocator, b->vertexBuffer, b->vertexBufferMemory);
            for (size_t i = 0; i < b->indexBuffer.size(); ++i)
                vmaDestroyBuffer(allocator, b->indexBuffer[i], b->indexBufferMemory[i]);
            //delete m;
        }
        //models.clear();

        for (auto it = resourseManager.models.begin(); it != resourseManager.models.end(); it++)
        {
            //delete it->second;
        }

        vkDestroyCommandPool(g_Device, commandPool, nullptr);
        vmaDestroyAllocator(allocator);
        vkDestroyDevice(g_Device, nullptr);

        if (enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(g_Instance, debugMessenger, nullptr);

        vkDestroyInstance(g_Instance, nullptr);
    }
    void LApp::releaseOpenGLResources()
    {
        LOG_CALL
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        //for (auto& t : LResourceManager::textures)
        //{
        //    auto diffuse = ((LResourceManager::OpenGLImage*)t.second.textures)->diffuse.texture;
        //    auto normals = ((LResourceManager::OpenGLImage*)t.second.textures)->normals.texture;

        //    if (diffuse)
        //    {
        //        glDeleteTextures(1, diffuse);
        //        delete diffuse;
        //    }
        //    if (normals)
        //    {
        //        glDeleteTextures(1, normals);
        //        delete normals;
        //    }
        //}
        LResourceManager::textures.clear();
        delete standartRectBuffer;
        delete standartCubeBuffer;
    }

    void LApp::releaseGlfwResources()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void LApp::releaseLGResources()
    {
        LOG_CALL 
    }

    void LApp::releaseResources()
    {
        LOG_CALL
        releaseLGResources();
        if (info.logFlags & ASYNC_LOG)
            LAsyncLogger::stop();
        if (info.api == L_VULKAN)
            releaseVulkanResources();
        else if (info.api == L_OPENGL)
            releaseOpenGLResources();

        for (auto p : primitives)
            delete p;
        primitives.clear();

        for (auto& m : models)
            delete m;
        models.clear();
    }


    void LApp::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        LOG_CALL
        userMouseButtonCallback(window, button, action, mods);
    }

    void LApp::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        LOG_CALL
        userCursorCallback(window, xpos, ypos);
        mouseCoords = { (float)xpos,(float)ypos };
    }

    void LApp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        LOG_CALL
        pressedKeys[key] = action != GLFW_RELEASE;
        userKeyCallback(window, key, scancode, action, mods);
        /*if (pressedKeys[GLFW_KEY_LEFT_ALT] && pressedKeys[GLFW_KEY_ENTER])
            switchScreenMode();*/
    }

    void LApp::character_callback(GLFWwindow* window, unsigned int codepoint)
    {
        LOG_CALL
        userCharacterCallback(window, codepoint);
    }

    void LApp::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        LOG_CALL
        userScrollCallback(window, xoffset, yoffset);
    }


#ifdef VULKAN
    
    VkResult LApp::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger)
    {
        LOG_CALL
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void LApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * pAllocator)
    {
        LOG_CALL
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    bool LApp::checkValidationLayerSupport()
    {
        LOG_CALL
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            if (!layerFound)
                return false;
        }

        return true;
    }

    void LApp::createInstance()
    {
        LOG_CALL
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Lizard Graphics";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Lizard Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        extensions.push_back("VK_KHR_android_surface");
#endif
        //extensions.push_back
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkValidationFeatureEnableEXT enabled[] = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        VkValidationFeaturesEXT features = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
        features.disabledValidationFeatureCount = 0;
        features.enabledValidationFeatureCount = 1;
        features.pDisabledValidationFeatures = nullptr;
        features.pEnabledValidationFeatures = enabled;
        features.pNext = createInfo.pNext;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        //debugCreateInfo.pNext = &features;

        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            createInfo.enabledLayerCount = 1;
            createInfo.pNext = &features;
            populateDebugMessengerCreateInfo(debugCreateInfo);
        }
        else 
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
        
        VkResult res = vkCreateInstance(&createInfo, nullptr, &g_Instance);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void LApp::pickPhysicalDevice()
    {
        LOG_CALL
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(g_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
            if (isDeviceSuitable(device)) 
            {
                g_PhysicalDevice = device;
                break;
            }

        if (g_PhysicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("failed to find a suitable GPU!");

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(g_PhysicalDevice, &deviceProperties);
        PRINTLN(deviceProperties.deviceName);
    }

    void LApp::createLogicalDevice()
    {
        LOG_CALL
        QueueFamilyIndices indices = findQueueFamilies(g_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        if (info.anisotropy)
            deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(g_PhysicalDevice, &createInfo, g_Allocator, &g_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        g_QueueFamily = indices.presentFamily.value();
        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    void LApp::createGraphicsPipeline()
    {
        LOG_CALL
        //baseShader = new LShaders::Shader("shaders//Vk_objV.spv", "shaders//Vk_objF.spv", this, false);
        lightShader.reset(new LShaders::VulkanShader(this,"shaders//Vk_litobjV.spv", "shaders//Vk_litobjF.spv"));
    }

    void LApp::createAllocator()
    {
        LOG_CALL
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
        allocatorInfo.physicalDevice = g_PhysicalDevice;
        allocatorInfo.device = g_Device;
        allocatorInfo.instance = g_Instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        vmaCreateAllocator(&allocatorInfo, &allocator);
    }


    void LApp::createRenderPass()
    {
        LOG_CALL
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = wd->SurfaceFormat.format;//wd->SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(g_Device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
            throw std::runtime_error("failed to create render pass!");
    }

    void LApp::createFramebuffers()
    {
        LOG_CALL
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView,
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = wd->Width;
            framebufferInfo.height = wd->Height;
            framebufferInfo.layers = 1;
            // framebufferInfo.flags = ;

            if (vkCreateFramebuffer(g_Device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create framebuffer!");
        }
    }

    void LApp::createCommandPool()
    {
        LOG_CALL
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(g_PhysicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(g_Device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create graphics command pool!");
    }

    void LApp::createUniformBuffers()
    {
        LOG_CALL
        uniformBuffers.resize(wd->ImageCount);
        uniformBuffersMemory.resize(wd->ImageCount);

        const size_t minUboAlignment = minUniformBufferOffsetAlignment;
        constexpr int materialSize = sizeof(int);

        dynamicAlignment = getPowerTwoAlign(sizeof(BaseVertexUBuffer));

        assert(dynamicAlignment > minUboAlignment && dynamicAlignment <= 256 && "error wrong alignment size");

        size_t bufferSize = info.poolSize * dynamicAlignment;
        testStructV.model = (glm::mat4*)alignedAlloc(bufferSize, dynamicAlignment);

        for (size_t i = 0; i < wd->ImageCount; i++)
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }

    void LApp::createDescriptorPool()
    {
        LOG_CALL
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(info.poolSize * wd->ImageCount);
        //poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        //poolSizes[1].descriptorCount = static_cast<uint32_t>(info.poolSize * wd->ImageCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(info.poolSize * wd->ImageCount);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(info.poolSize * wd->ImageCount);

        if (vkCreateDescriptorPool(g_Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void LApp::createImageViews()
    {
        LOG_CALL
        swapChainImageViews.resize(wd->ImageCount);

        for (uint32_t i = 0; i < wd->ImageCount; i++)
             createImageView(wd->Frames[i].Backbuffer, wd->SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, swapChainImageViews[i],1);
    }

    void LApp::createDepthResources()
    {
        LOG_CALL
        depthFormat = findDepthFormat();
        createImage(wd->Width, wd->Height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory,1);
        createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageView,1);
    }

    VkFormat LApp::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        LOG_CALL
        for (VkFormat format : candidates) 
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(g_PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat LApp::findDepthFormat()
    {
        LOG_CALL
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    bool LApp::hasStencilComponent(VkFormat format)
    {
        LOG_CALL
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void LApp::createDescriptorSets()
    {
        LOG_CALL
        std::vector<VkDescriptorSetLayout> layouts(info.poolSize * wd->ImageCount, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(info.poolSize * wd->ImageCount);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(wd->ImageCount * info.poolSize);
        if (vkAllocateDescriptorSets(g_Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }


     for (size_t i = 0; i < info.poolSize; ++i)
        for (size_t j = 0; j < wd->ImageCount; j++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[j];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(BaseVertexUBuffer);

            VkDescriptorBufferInfo bufferInfo2{};
            bufferInfo2.buffer = uniformBuffers[j];
            bufferInfo2.offset = bufferInfo.range;
            bufferInfo2.range = sizeof(int);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = dummyTexture;
            imageInfo.sampler = textureSamplers[0];

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i* wd->ImageCount + j];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            //descriptorWrites[1].dstSet = descriptorSets[i * wd->ImageCount + j];
            //descriptorWrites[1].dstBinding = 1;
            //descriptorWrites[1].dstArrayElement = 0;
            //descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            //descriptorWrites[1].descriptorCount = 1;
            //descriptorWrites[1].pBufferInfo = &bufferInfo2;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i * wd->ImageCount + j];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(g_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void LApp::createDescriptorSetLayout()
    {
        LOG_CALL
            VkDescriptorSetLayoutBinding vertexLayoutBinding{};
            vertexLayoutBinding.binding = 0;
            vertexLayoutBinding.descriptorCount = 1;
            vertexLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            vertexLayoutBinding.pImmutableSamplers = nullptr;
            vertexLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

            //VkDescriptorSetLayoutBinding materialLayoutBinding{};
            //materialLayoutBinding.binding = 1;
            //materialLayoutBinding.descriptorCount = 1;
            //materialLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            //materialLayoutBinding.pImmutableSamplers = nullptr;
            //materialLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            std::array<VkDescriptorSetLayoutBinding, 2> bindings = 
            { vertexLayoutBinding, samplerLayoutBinding };
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(g_Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
    }

    void LApp::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, 
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VmaAllocation& imageMemory, uint32_t miplevels)
    {
        LOG_CALL
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = miplevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(g_Device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }
        
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(g_Device, image, &memRequirements);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = properties;
        allocInfo.preferredFlags =  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        allocInfo.memoryTypeBits = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

        uint32_t memoryTypeIndex;
        vmaFindMemoryTypeIndex(allocator, allocInfo.memoryTypeBits, &allocInfo, &memoryTypeIndex);

        VmaAllocationInfo info;

        vmaAllocateMemoryForImage(allocator, image, &allocInfo, &imageMemory,&info);
        vmaBindImageMemory(allocator, imageMemory, image);
        //vkBindImageMemory(g_Device, image, imageMemory->GetMemory(), 0);
    }

    void LApp::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& view, uint32_t miplevels)
    {
        LOG_CALL
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.format = format;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = miplevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        //VkImageView imageView;
        if (vkCreateImageView(g_Device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void LApp::createTextureSampler(VkSampler& sampler, size_t mipLevels)
    {
        LOG_CALL
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(g_PhysicalDevice, &properties);

        minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;
        
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = info.anisotropy == L_TRUE;

        while (info.anisotropy > properties.limits.maxSamplerAnisotropy)
            info.anisotropy >>= 1;

        samplerInfo.maxAnisotropy = info.anisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(g_Device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void LApp::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
    {
        LOG_CALL
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(g_PhysicalDevice, imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    VkCommandBuffer LApp::beginSingleTimeCommands()
    {
        LOG_CALL
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(g_Device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void LApp::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        LOG_CALL
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(g_Queue);

        vkFreeCommandBuffers(g_Device, commandPool, 1, &commandBuffer);
    }

    void LApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
        VmaAllocation& allocation)
    {
        LOG_CALL
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

        bufferInfo.size = size;
        bufferInfo.usage = usage;
        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocCreateInfo.requiredFlags = properties;
        allocCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        allocCreateInfo.memoryTypeBits = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        
        vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
    }

    void LApp::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset)
    {
        LOG_CALL
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.dstOffset = offset;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void LApp::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t miplevels)
    {
        LOG_CALL
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = miplevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    void LApp::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        LOG_CALL
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};

        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    /*void LApp::recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(window_, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
    }*/

    /*void LApp::cleanupSwapChain()
    {
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        delete baseShader;
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
            
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }*/

    void LApp::mapUniformData()
    {
        LOG_CALL
        for (size_t i = 0; i < wd->ImageCount; ++i)
            vmaMapMemory(allocator, uniformBuffersMemory[i], &uniformsMem[i]);
    }

    void LApp::updateUniformBuffer(uint32_t currentImage, uint32_t objectNum, LWidget* w)
    {
        LOG_CALL
        //if (info.api == L_VULKAN)
        //{
        //    if (w->textures)
        //    {
        //        VkImageView texture = (VkImageView)w->((LResourceManager::VulkanImage*)textures.get());
        //        updateTexture(texture, currentImage, objectNum, w->getMipLevels());
        //        if (w->changed <= LWidget::ONE_BUFFER_TO_CHANGE)
        //            w->texture = VkImageView();
        //    }

        //    if (w->changed == LWidget::UNMODIFIED)
        //        return;

        //    w->changed--;
        //    glm::mat4* modelMat = (glm::mat4*)(((char*)testStructV.model + (objectNum * dynamicAlignment)));

        //    *modelMat = ((LWRectangle*)w)->calculateModelMatrix();
        //    glm::vec4* color = (glm::vec4*)((char*)modelMat + sizeof(glm::mat4));
        //    const auto col = w->getColor();
        //    *color = { col.x, col.y, col.z, w->getTransparency() };
        //    int* materialNum = (int*)((char*)color + sizeof(glm::vec4));
        //    *materialNum = w->getMaterial();

        //    const int material = w->getMaterial();

        //    const auto offset = (char*)uniformsMem[currentImage] + objectNum * dynamicAlignment;
        //    //const auto materialOffset = offset + sizeof(BaseVertexUBuffer);
        //    memcpy(offset, modelMat, sizeof(BaseVertexUBuffer));
        //    //memcpy(materialOffset, &material, sizeof(int));
        //}   

//        else if (info.api == L_OPENGL)
//        {
//            auto shader = (LShaders::OpenGLShader*)w->getShader();
//            const auto projView = getProjectionMatrix() * getViewMatrix();
//            //        const float t = sqrt(3);
////const glm::vec3 viewPos = app->getViewPoint() + app->getViewRadius() * glm::vec3(t, t, t);
////glUniform3f(glGetUniformLocation(shader, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
//   glUniformMatrix4fv(glGetUniformLocation(shader->getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(((LWRectangle*)w)->calculateModelMatrix()));
//   glUniformMatrix4fv(glGetUniformLocation(shader->getShaderProgram(), "projView"), 1, GL_FALSE, glm::value_ptr(projView));
////glUniform4f(glGetUniformLocation(shader, "color_"), color_.x, color_.y, color_.z, transparency_);
//        }
    }

    void LApp::updateTexture(VkImageView& view, uint32_t currentImage, uint32_t objectNum, size_t mipLevels)
    {
        LOG_CALL
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[currentImage];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(BaseVertexUBuffer);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = view;
        imageInfo.sampler = textureSamplers[mipLevels-1];

        VkDescriptorBufferInfo bufferInfo2{};
        bufferInfo2.buffer = uniformBuffers[currentImage];
        bufferInfo2.offset = bufferInfo.range;
        bufferInfo2.range = sizeof(int);

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[objectNum * wd->ImageCount + currentImage];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //descriptorWrites[1].dstSet = descriptorSets[objectNum * wd->ImageCount + currentImage];
        //descriptorWrites[1].dstBinding = 1;
        //descriptorWrites[1].dstArrayElement = 0;
        //descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        //descriptorWrites[1].descriptorCount = 1;
        //descriptorWrites[1].pBufferInfo = &bufferInfo2;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[objectNum * wd->ImageCount + currentImage];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(g_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    void LApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo)
    {
        LOG_CALL
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        //createInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
    }

    void LApp::setupDebugMessenger()
    {
        LOG_CALL
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(g_Instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    std::vector<const char*> LApp::getRequiredExtensions()
    {
        LOG_CALL
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME); 
        if (enableValidationLayers) 
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        //extensions.push_back
        return extensions;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL LApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
    {
        LOG_CALL
        PRINTLN("validation layer: ",pCallbackData->pMessage, "\n");
        return VK_FALSE;
    }

    bool LApp::isDeviceSuitable(VkPhysicalDevice device)
    {
        LOG_CALL
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    bool LApp::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        LOG_CALL
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    LApp::QueueFamilyIndices LApp::findQueueFamilies(VkPhysicalDevice device)
    {
        LOG_CALL
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    LApp::SwapChainSupportDetails LApp::querySwapChainSupport(VkPhysicalDevice device)
    {
        LOG_CALL
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR LApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        LOG_CALL
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR LApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        LOG_CALL
        if (info.vsync == L_FALSE)
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        
        PRINTLN("vsync is not supported!");
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D LApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
    {
        LOG_CALL
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window_, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    uint32_t LApp::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        LOG_CALL
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(g_PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

#endif

    void LApp::setupVulkan()
    {
        LOG_CALL
        VkResult err;
        wd = &g_MainWindowData;

        createInstance();
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        setupDebugMessenger();
#endif
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createAllocator();

        glfwGetFramebufferSize(window_, (int*)(&info.wndWidth), (int*)(&info.wndHeight));
        SetupVulkanWindow(wd, surface, info.wndWidth, info.wndHeight);

        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createCommandPool();

        createDepthResources();
        createFramebuffers();

        LResourceManager::setApp(this);
        //size_t dummy;
        //dummyTexture = (VkImageView)LResourceManager::loadTexture(notexture, notextureSize, "dummy", dummy);

        for (size_t i = 1; i < possibleMipLevels+1; ++i)
            createTextureSampler(textureSamplers[i-1], i);
        createUniformBuffers();
        mapUniformData();

        createDescriptorPool();
        createDescriptorSets();
        standartRectBuffer = new LRectangleBuffer(this);

        {
            VkDescriptorPoolSize pool_sizes[] =
            {
                // ImGUI
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },

                // Lizard Graphics
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
            check_vk_result(err);
        }
    }

    void LApp::SetupVulkanWindow(ImGui_ImplVulkanH_Window * wd, VkSurfaceKHR surface, int width, int height)
    {
        LOG_CALL
        wd->Surface = surface;

        // Check for WSI support
        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
        if (res != VK_TRUE)
        {
            fprintf(stderr, "Error no WSI support on physical device 0\n");
            exit(-1);
        }

        // Select Surface Format
        wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
#define IMGUI_UNLIMITED_FRAME_RATE
        // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

        // Create SwapChain, RenderPass, Framebuffer, etc.
        IM_ASSERT(g_MinImageCount >= 2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
    }

    void LApp::createSurface()
    {
        LOG_CALL
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            ANativeWindow* w;
        VkAndroidSurfaceCreateInfoKHR createInfo
        {
              .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
              .pNext = nullptr,
              .flags = 0,
              .window = w,
        };
        if (!vkCreateAndroidSurfaceKHR(g_Instance, &createInfo, nullptr,
            &surface))
            throw std::runtime_error("failed to create window surface!");
#else
            if (glfwCreateWindowSurface(g_Instance, window_, nullptr, &surface) != VK_SUCCESS)
                throw std::runtime_error("failed to create window surface!");
#endif
    }
    
//    void LApp::CleanupVulkan()
//    {
//        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
//
//#ifdef IMGUI_VULKAN_DEBUG_REPORT
//        // Remove the debug report callback
//        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
//        vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
//#endif // IMGUI_VULKAN_DEBUG_REPORT
//
//        vkDestroyDevice(g_Device, g_Allocator);
//        vkDestroyInstance(g_Instance, g_Allocator);
//    }

    void LApp::FrameRender(ImGui_ImplVulkanH_Window * wd, ImDrawData* draw_data)
    {
        LOG_CALL
        VkResult err;

        VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR)
        {
            g_SwapChainRebuild = true;
            return;
        }
        check_vk_result(err);

        ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
        {
            err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
            check_vk_result(err);

            err = vkResetFences(g_Device, 1, &fd->Fence);
            check_vk_result(err);
        }
        {
            err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
            check_vk_result(err);
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
            check_vk_result(err);
        }
        {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

            //info.renderPass = wd->RenderPass;
            //info.framebuffer = fd->Framebuffer;
            info.renderPass = renderPass;
            info.framebuffer = swapChainFramebuffers[wd->FrameIndex];
            info.renderArea.extent.width = wd->Width;
            info.renderArea.extent.height = wd->Height;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = wd->ClearValue.color;
            clearValues[1].depthStencil = { 1.0f, 0 };
            info.clearValueCount = static_cast<uint32_t>(clearValues.size());
            info.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        const auto viewProj = getProjectionMatrix() * getViewMatrix();

        LApp::ShaderConstants shaderCnst
        {
            viewProj,
            cameraPos,
            lightPos,
            {0.9f,0.9f,0.9f},
            {0.5f,0.5f,0.5f},
            //info.lighting,
        };

        if (primitives.size())
        {
            auto obj = primitives[0];
            auto shader = (LShaders::VulkanShader*)obj->getShader();

            VkBuffer vertexBuffers[] = { ((LWRectangle*)obj)->buffer->getVertBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindPipeline(fd->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getGraphicsPipeline());

            vkCmdBindVertexBuffers(fd->CommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(fd->CommandBuffer, ((LWRectangle*)obj)->buffer->getIndBuffer(), 0, VK_INDEX_TYPE_UINT16);

            vkCmdPushConstants(fd->CommandBuffer, shader->getPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LApp::ShaderConstants), &shaderCnst);

            for (size_t i = 0; i < primitives.size(); ++i)
                primitives[i]->draw(fd->CommandBuffer, wd->FrameIndex);
        }

        if (models.size())
        {
            auto obj = models[0];
            auto shader = (LShaders::VulkanShader*)obj->getShader();

            vkCmdBindPipeline(fd->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getGraphicsPipeline());
            vkCmdPushConstants(fd->CommandBuffer, shader->getPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LApp::ShaderConstants), &shaderCnst);

            for (size_t i = 0; i < models.size(); ++i)
                models[i]->draw(fd->CommandBuffer, wd->FrameIndex);
        }

        // Record dear imgui primitives into command buffer
        if (drawUI_)
            ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &image_acquired_semaphore;
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &fd->CommandBuffer;
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &render_complete_semaphore;

            err = vkEndCommandBuffer(fd->CommandBuffer);
            check_vk_result(err);
            err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
            check_vk_result(err);
        }
    }

    void LApp::FramePresent(ImGui_ImplVulkanH_Window * wd)
    {
        LOG_CALL
        if (g_SwapChainRebuild)
            return;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &render_complete_semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd->Swapchain;
        info.pImageIndices = &wd->FrameIndex;
        VkResult err = vkQueuePresentKHR(g_Queue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR)
        {
            g_SwapChainRebuild = true;
            return;
        }
        check_vk_result(err);
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount;
    }


    void LApp::initTextures()
    {
        LOG_CALL
        //initTextures(rectangles);
    }

    std::array<VkVertexInputAttributeDescription, 3> LApp::Vertex::getAttributeDescriptions()
    {
        LOG_CALL
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

        if (info.lighting == L_TRUE)
        {
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, normals);
        }

        return attributeDescriptions;
    }
}

void LGraphics::LApp::InstantPoolCubes::setApp(LApp* app)
{
    this->app = app;
    uniformBuffersSize = app->info.poolSize;
    uniformBuffers = new LWidget::WidgetUniforms[uniformBuffersSize];
    objs = new LCube* [uniformBuffersSize];
    std::fill(objs, objs + uniformBuffersSize, nullptr);
}

void LGraphics::LApp::InstantPoolCubes::draw()
{
    auto shader = (LShaders::OpenGLShader*)app->getStandartShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();

    const auto proj = app->getProjectionMatrix();
    const auto view = app->getViewMatrix();

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));

    if (!app->drawingInShadow)
    {
        glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
    }

    LCube* obj = nullptr;
    bool modified = false;
    for (size_t i = 0; i < uniformBuffersSize; ++i)
    {
        if (objs[i])
        {
            obj = objs[i];
            if (obj->isChanged())
            {
                modified = true;
                obj->changed = LWidget::UNMODIFIED;
                obj->refreshModel();
                uniformBuffers[i].model = obj->getModelMatrix();
            }
        }
        //else
        //    uniformBuffers[i].id = -1;
    }
    if (modified)
        updateBuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *(GLuint*)obj->getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->depthMap);
    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, uniformBuffersSize);
}

LGraphics::LApp::InstantPoolCubes::~InstantPoolCubes()
{
    delete[] uniformBuffers;
    delete[] objs;
    glDeleteBuffers(1, &vbo);
}

void LGraphics::LApp::InstantPoolCubes::updateBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::WidgetUniforms) * uniformBuffersSize, uniformBuffers, GL_STATIC_DRAW);

    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    const size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    //glVertexAttribDivisor(7, 1);
    //glVertexAttribDivisor(8, 1);
    //glVertexAttribDivisor(9, 1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LGraphics::LApp::InstantPoolCubes::initPool()
{
    glGenBuffers(1, &vbo);
    updateBuffer();
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR
bool initialize(android_app* app)
{
    if (!InitVulkan()) {
        LOGE("Vulkan is unavailable, install vulkan and restart");
        return false;
    }
}

void android_main(android_app* app)
{
    app->onAppCmd = handle_cmd;

    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(initialized_ ? 1 : 0, nullptr, &events,
            (void**)&source) >= 0) {
            if (source != NULL) source->process(app, source);
        }
    } while (app->destroyRequested == 0);
}

void terminate(void)
{
    initialized_ = false;
}

void handle_cmd(android_app* app, int32_t cmd)
{
    switch (cmd) 
    {
    case APP_CMD_INIT_WINDOW:
        // The window is being shown, get it ready.
        initialize(app);
        break;
    case APP_CMD_TERM_WINDOW:
        // The window is being hidden or closed, clean it up.
        terminate();
        break;
    default:
        LOGI("event not handled: %d", cmd);
    }
}

#endif