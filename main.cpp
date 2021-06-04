/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "interfaces.h"
#include "LCube.h"
#include "LSkyBox.h"
#include "LRectangleMirror.h"

#include <ctime>

using namespace LGraphics;

int main(int argc, char** argv)
{
    const size_t poolSize = 10000;

    LAppCreateInfo info;

    info.api = L_OPENGL;
    info.projection = L_PERSPECTIVE;
    info.saveObjects = L_FALSE;
    info.loadObjects = L_FALSE;
    info.lighting = L_TRUE;
    info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;
    info.anisotropy = 16;
    info.MSAA = 4;
    info.vsync = L_FALSE;
    info.poolSize = poolSize;
    //info.wndHeight = 600;
    //info.wndWidth = 800;
    const auto spread = 500;
    LApp app(info);
    srand(time(0));
    ImGuiInterface interface_(&app);
    auto f = std::bind(&ImGuiInterface::imguiInterface, &interface_);
    app.setImgui(f);
    LCube* c;
    if (!info.loadObjects)
    {
        for (size_t i = 0; i < poolSize - 1; ++i)
        {
            c = new LCube(&app, "textures/image1.bmp");
            c->move(rand() % 100, rand() % spread, rand() % spread);
            c->rotateX(rand() % spread);
            c->rotateY(rand() % spread);
            c->rotateZ(rand() % spread);
        }

        //for (size_t i = 0; i < 20; ++i)
        //    for (size_t j = 0; j < 20; ++j)
        //{
        //    auto c = new LWRectangle(&app, "textures/image1.bmp");
        //    c->move(i, -0.5f, j);
        //    c->rotateX(90.0f);
        //    c->scale(1.0f, 1.0f, 1.0f);
        //}

        c = new LCube(&app, "textures/gold.jpg");
        c->scale(0.1f, 0.1f, 0.1f);
        c->move(7.5f, 0.0f, 7.5f);
        //new LSkyBox(&app, {
        //    "textures/skyboxes/Spacebox4/_right.png",
        //    "textures/skyboxes/Spacebox4/_left.png",
        //    "textures/skyboxes/Spacebox4/_top.png",
        //    "textures/skyboxes/Spacebox4/_bottom.png",
        //    "textures/skyboxes/Spacebox4/_front.png",
        //    "textures/skyboxes/Spacebox4/_back.png", });
    }

    app.setBeforeDrawingFunc([&]()
        {
            if (app.isCursorEnabled())
                return;

            static auto cMove = c->getMove();
            const auto move = c->getMove();

            const float radius = poolSize + 13.0f;
            const float shiftCoef = -1.5f;
            glm::vec3 shift = glm::normalize(move - glm::vec3(7.5f, 0.0f, 7.5f)) * shiftCoef;


            GLfloat camX = sin(glfwGetTime());
            GLfloat camZ = cos(glfwGetTime());

            c->move(cMove.x + radius * camX, c->getMove().y, cMove.z + radius * camZ);

            const float step = 0.01f;
            //c->rotateY(step);

            const float cameraSpeed = 0.002f;
            glm::vec3 cameraPos = app.getCameraPos();
            const auto cameraFront = app.getCameraFront();
            const auto cameraUp = app.getCameraUp();

            const float tempY = cameraPos.y;

            if (app.isPressed(GLFW_KEY_W))
                cameraPos += cameraSpeed * cameraFront;
            if (app.isPressed(GLFW_KEY_S))
                cameraPos -= cameraSpeed * cameraFront;
            if (app.isPressed(GLFW_KEY_A))
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (app.isPressed(GLFW_KEY_D))
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

            cameraPos.y = 0.0f;
            app.setCameraPos(cameraPos);
            app.setLightPos(glm::vec3(move.x + shift.x, move.y + 2.5f, move.z + shift.z));
        });

    app.setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
        {
            if (app.isPressed(GLFW_KEY_LEFT_CONTROL))
                app.setCursorEnabling(!app.isCursorEnabled());
            //if (app.isPressed(GLFW_KEY_G))
            //    app.switchRendererTo(app.getAppInfo().api == L_VULKAN ? L_OPENGL : L_VULKAN);
        });

    app.setUserCursorCallback([&](GLFWwindow* window, double xpos, double ypos)
        {
        if (app.isCursorEnabled())
            return;

            float xoffset = xpos - app.getMouseCoords().x;
            float yoffset = ypos - app.getMouseCoords().y;

            if (app.cursorModeSwitched)
            {
                xoffset = 0;
                yoffset = 0;
                app.cursorModeSwitched = false;
            }

            const float sensitivity = 0.08f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            app.setYaw(app.getYaw() + xoffset);
            app.setPitch(app.getAppInfo().api == L_OPENGL? app.getPitch() - yoffset : app.getPitch() + yoffset);

            auto pitch = app.getPitch();
            auto yaw = app.getYaw();

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            front.y = sin(glm::radians(pitch));
            front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            app.setCameraFront(glm::normalize(front));
        });

    app.setUserMouseButtonCallback([&](GLFWwindow* window, int button, int action, int mods) 
    {
        if (app.isCursorEnabled())
        {
            if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
            {
                auto coords = app.getMouseCoords();
                coords.y = app.getWindowSize().y - coords.y;
                const auto id = app.getObjectsOnScreen()[app.getWindowSize().x * (int)coords.y + (int)coords.x];
                if (id >= 0)
                    app.findByLID(id)->rotateX(45.0f);
            }
        }
    });

    app.setLightPos(app.getCameraPos());
    app.loop();
    return 0;
}