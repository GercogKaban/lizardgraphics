/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "interfaces.h"

using namespace LGraphics;

LApp* app;

void logick()
{
    try
    {
        const size_t poolSize = 100;

        LAppCreateInfo info;
        info.anisotropy = 16;
        info.projection = L_PERSPECTIVE;
        info.saveObjects = L_TRUE;
        info.loadObjects = L_FALSE;
        info.lighting = L_TRUE;
        info.poolSize = poolSize;
        info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;

        app = new LApp(info);

        ImGuiInterface interface_(app);
        auto f = std::bind(&ImGuiInterface::imguiInterface, &interface_);
        app->setImgui(f);

        if (!info.loadObjects)
        {
            auto m1 = new LModel(app,
                "models/dog/Wolf_obj.obj",      // model path
                "models/dog/Australian_Cattle_Dog_dif.jpg", nullptr, true);     // texture path

            m1->rotateX(90.0f);
            m1->rotateZ(180.0f);
            m1->move(0.0f, 10.0f, 0.0f);
        }

        app->setBeforeDrawingFunc([&]()
            {
                if (app->isCursorEnabled())
                    return;
                const float cameraSpeed = 0.002f;

                glm::vec3 cameraPos = app->getCameraPos();
                const auto cameraFront = app->getCameraFront();
                const auto cameraUp = app->getCameraUp();

                const float tempY = cameraPos.y;

                if (app->isPressed(GLFW_KEY_W))
                    cameraPos += cameraSpeed * cameraFront;
                if (app->isPressed(GLFW_KEY_S))
                    cameraPos -= cameraSpeed * cameraFront;
                if (app->isPressed(GLFW_KEY_A))
                    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                if (app->isPressed(GLFW_KEY_D))
                    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

                cameraPos.y = 0.0f;
                app->setCameraPos(cameraPos);
            });

        app->setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
            {
                if (app->isPressed(GLFW_KEY_LEFT_CONTROL))
                    app->setCursorEnabling(!app->isCursorEnabled());
            });

        app->setUserCursorCallback([&](GLFWwindow* window, double xpos, double ypos)
            {
                float xoffset = xpos - app->getMouseCoords().x;
                float yoffset = ypos - app->getMouseCoords().y;

                if (app->cursorModeSwitched)
                {
                    xoffset = 0;
                    yoffset = 0;
                    app->cursorModeSwitched = false;
                }

                const float sensitivity = 0.08f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                app->setYaw(app->getYaw() + xoffset);
                app->setPitch(app->getPitch() + yoffset);

                auto pitch = app->getPitch();
                auto yaw = app->getYaw();

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                glm::vec3 front;
                front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
                front.y = sin(glm::radians(pitch));
                front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
                app->setCameraFront(glm::normalize(front));
            });

        app->setLightPos(glm::vec3(-12.5f, 0.0f, 0.0f));
    }

    catch (std::exception& exception)
    {
        app->emergencyStop(exception);
    }
    app->loop();
}

int main(int argc, char** argv)
{
#ifdef WIN32
    __try
    {
        logick();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        app->emergencyStop(exception_code());
    }
#else
    logick();
#endif
    return 0;
}