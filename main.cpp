/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "interfaces.h"

using namespace LGraphics;

int main()
{
    LAppCreateInfo info;

    const size_t poolSize = 100;
    info.anisotropy = 16;
    info.projection = L_PERSPECTIVE;
    info.saveObjects = L_TRUE;
    info.loadObjects = L_TRUE;
    info.poolSize = poolSize;

    LApp app(info);

    ImGuiInterface interface_(&app);
    auto f = std::bind(&ImGuiInterface::imguiInterface, &interface_);
    app.setImgui(f);

    if (!info.loadObjects)
    {
        auto m1 = new LModel(&app,
            "models/dog/Wolf_obj.obj",      // model path
            "models/dog/Australian_Cattle_Dog_dif.jpg", nullptr, true);     // texture path

        m1->rotateX(90.0f);
        m1->rotateZ(180.0f);
        m1->move(0.0f, 10.0f, 0.0f);
    }

    //auto r = new LWRectangle(&app,"");
   
    app.setBeforeDrawingFunc([&]()
        {
            if (app.isCursorEnabled())
                return;
            const float cameraSpeed = 0.005f;

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
        });

    app.setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
        {
            if (app.isPressed(GLFW_KEY_LEFT_CONTROL))
                app.setCursorEnabling(!app.isCursorEnabled());
        });

    //app.drawUI(false);
    app.loop();
    return 0;
}