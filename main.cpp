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
    const size_t poolSize = 20;

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

#ifndef NDEBUG
    info.texturesQuality = LOW;
#else
    info.texturesQuality = HIGH;
#endif

    //info.freeThreads = 1;

    const auto spread = 5;
    LApp app(info);
    srand(time(0));
    ImGuiInterface interface_(&app);
    auto f = std::bind(&ImGuiInterface::imguiInterface, &interface_);
    app.setImgui(f);
    LCube* c;

    app.fog.density = 0.02f;
    app.fog.color = glm::vec3(211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f);
    app.fog.isEnabled = true;

    auto s = new LPointLight(&app);
    s->setAmbient({0.9f,0.9f,0.9f});
   
    //s->turnOnShadowCalc();
    //s->setRadius(25);

    if (!info.loadObjects)
    {   
        //auto m = new LModel(&app, {"Walking.fbx"});
        //m->scale(0.01f, 0.01f, 0.01f);
        //m->move(0.0, 0.0f, 0.0f);
        //m->rotateX(180.0f);
        //m->rotateZ(90.0f);
        //for (size_t i = 0; i < 1; ++i)
        {   
            new LCube(&app, {"red_bricks_04.jpg"});
            //c->move(rand() % spread, -0.2f, rand() % spread);
            //c->scale(1.0f, 1.0f, 1.0f);
            //c->rotateX(180.0f); 
            //c->move(0.0f, -1.0f, 0.0f);
        }

            c = new LCube(&app, { "red_bricks_04.jpg" });

        
        //for (size_t i = 0; i < 20; ++i)
        //    for (size_t j = 0; j < 20; ++j)
        //{
        //    auto c = new LWRectangle(&app, "textures/image1.bmp");
        //    c->move(i, -0.5f, j);
        //    c->rotateX(90.0f);
        //    c->scale(1.0f, 1.0f, 1.0f);
        //}

        //c = new LCube(&app, "textures/gold.jpg");
        //c->scale(0.1f, 0.1f, 0.1f);
        //c->move(7.5f, 0.0f, 7.5f);
        //new LSkyBox(&app, {"red"});
    }

    bool flag = true;
    app.setBeforeDrawingFunc([&]()
        {
            if (app.isCursorEnabled())
                return;

            static auto cMove = c->getMove();
            const auto move = c->getMove();

            const float radius = poolSize + 13.0f;
            const float shiftCoef = -1.05f;
            glm::vec3 shift = glm::normalize(move - s->getPosition()) * shiftCoef;

            int i = 0;
            //auto p = app.getPrimitives()[L_CUBE];
            //while (i < poolSize)
            //{
            //    p[i]->rotateX(1.0f);
            //    p[i]->rotateY(1.0f);
            //    p[i]->rotateZ(1.0f);
            //    p[i]->move(p[i]->getMove() + 0.01f);
            //    p[i]->scale(p[i]->getScale() + 0.001f);
            //    i++;
            //}

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
            //if (flag)
            {
                //s->setDirection(cameraFront);
                s->setPosition(cameraPos);
            }
        });

    app.setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
        {
            if (app.isPressed(GLFW_KEY_LEFT_CONTROL))
                app.setCursorEnabling(!app.isCursorEnabled());
            if (app.isPressed(GLFW_KEY_X))
                app.setNormalMapping(!app.getNormalMapping());
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
            //if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
            //{
            //    auto coords = app.getMouseCoords();
            //    coords.y = app.getWindowSize().y - coords.y;
            //    const auto id = app.getObjectsOnScreen()[app.getWindowSize().x * (int)coords.y + (int)coords.x];
            //    if (id >= 0)
            //        app.findByLID(id)->rotateX(45.0f);
            //}
        }
    });

    //app.setDirLightPos(app.getCameraPos());
    app.loop();
    return 0;
}