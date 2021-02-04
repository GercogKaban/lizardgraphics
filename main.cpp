/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LWRectangle.h"
#include "LModel.h"
#include "LTimer.h"

using namespace LGraphics;

int main()
{
    LAppCreateInfo info;

    info.anisotropy = 16;
    info.projection = L_PERSPECTIVE;

    LApp app(info);

    //const size_t poolSize = app.getPoolSize();

    //float offsetX = 0.0f;
    //for (size_t i = 0; i < poolSize-1; ++i)
    //{
        //auto w = new LWRectangle(&app);
    //    w->color(255, 255, 255);
    //    //w->scale(0.1f, 0.1f, 0.1f);
    //    w->move(0.0f, 0.0f, 0.0f);
        //w->bindTexture("arbiter.png");
    //}
        
    app.setImgui([&]()
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    });


    auto m1 = new LModel(&app, 
    "models/dog/13463_Australian_Cattle_Dog_v3.obj",      // model path
    "models/dog/Australian_Cattle_Dog_dif.jpg");     // texture path

    //auto test = new LWRectangle(&app);

    //auto m1 = new LModel(&app, 
    //    "models/Handgun/Handgun.obj",                  // model path
    //    "models/Handgun/handgun_C.jpg");     // texture path
     
    //m1->setMeshDrawing(5, false);
    //m1->setMeshDrawing(6, false);
    //m1->setMeshDrawing(2, false);
    //m1->setMeshDrawing(0, false);
    m1->rotateX(90.0f);
    m1->rotateZ(180.0f);
    m1->move(0.0f, 10.0f, 0.0f);
    //app.setBeforeDrawingFunc ([&]() 
    //    {
    //    });

    app.setAfterDrawingFunc([&]()
        {
            const float cameraSpeed = 0.03f;

            glm::vec3 cameraPos = app.getCameraPos();
            const auto cameraFront = app.getCameraFront();
            const auto cameraUp = app.getCameraUp();

            if (app.isPressed(GLFW_KEY_W))
                cameraPos += cameraSpeed * cameraFront;
            if (app.isPressed(GLFW_KEY_S))
                cameraPos -= cameraSpeed * cameraFront;
            if (app.isPressed(GLFW_KEY_A))
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (app.isPressed(GLFW_KEY_D))
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

            app.setCameraPos(cameraPos);
        });

    app.setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
        {
            if (app.isPressed(GLFW_KEY_LEFT_CONTROL))
                app.setCursorEnabling(!app.isCursorEnabled());
        });
    //m1->rotateZ(180);
    //m1->rotateY(45.0f);

    //LTimer animation([&]() 
    //    {
    //        m1->setMeshDrawing(5, !m1->getMeshDrawing(5));
    //    },1000, true);

    //LTimer animation1([&]()
    //    {
    //        m1->setMeshDrawing(5, !m1->getMeshDrawing(5));
    //    },100,true);

    app.loop();

    //animation.stop();
    //animation1.stop();
    return 0;
}