/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "interfaces.h"
#include "LModel.h"
#include "LCylinder.h"
#include "LCube.h"
#include "LCone.h"
#include "LSphere.h"
#include "LIcosphere.h"
#include "LTorus.h"
#include "LPlane.h"
#include "LCylinder.h"
#include "interpretator.h"

#include <ctime>

using namespace LGraphics;

int main(int argc, char** argv)
{
    LAppInitialCreateInfo info;

    info.api = L_OPENGL;
    info.renderingType = DEFERRED;
    info.projection = L_PERSPECTIVE;
    info.picking = L_TRUE;
    info.redactorMode = L_TRUE;
    info.saveObjects = L_FALSE;
    info.loadObjects = L_FALSE;
    info.lighting = L_TRUE;
    info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | CONSOLE_RELEASE_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;
    info.anisotropy = 16;
    info.MSAA = 4;
    info.vsync = L_FALSE;
    //info.ssr = L_FALSE;

#ifndef NDEBUG
    info.texturesQuality = LOW;
    info.loading = FAST;
    info.shadowsHeight = 2048;
    info.shadowsWidth = 2048;
#else
    info.texturesQuality = HIGH;
    info.loading = FAST;
    info.shadowsHeight = 4096;
    info.shadowsWidth = 4096;
#endif

    const auto spread = 5;
    LApp app(info);
    srand(time(0));
    ImGuiInterface interface_(&app);
    auto f = std::bind(&ImGuiInterface::imguiInterface, &interface_);
    app.setImgui(f);

    app.fog.density = 0;
    app.fog.color = glm::vec3(211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f);
    app.fog.isEnabled = false;

    auto dirLight = new LDirectionalLight(&app);
    dirLight->setPosition(glm::vec3(0.0f, 5.0f, 0));
    dirLight->setDirection(glm::vec3(4, 0, 4));

    //auto pointLight = new LPointLight(&app);
    //pointLight->setShadowsCalculating(true);

    float yOffset = -0.3f;
    if (!info.loadObjects)
    {
        {
            for (size_t i = 0; i < 10; ++i)
                for (size_t j = 0; j < 2; ++j)
                {
                    LPlane* p;
                    p = new LPlane(&app, { "Rocks011.jpg" });
                    p->rotateX(270.0f);
                    p->move(i, yOffset, (float)j);
                    p->setParallaxMapping(false);
                    //p->setNormalMapping(false);
                }

            for (size_t i = 0; i < 2; ++i)
                for (size_t j = 0; j < 2; ++j)
                {
                    LPlane* p;
                    p = new LPlane(&app, { "Rocks011.jpg" });
                    p->rotateX(270.0f);
                    p->rotateY(180.0f);
                    p->move(i, yOffset + 1.0f, (float)j);
                    p->setParallaxMapping(false);
                    //p->setNormalMapping(false);
                }

            const float off = 1.5f;
            const float startX = 1.0f;

            auto cyl = new LCylinder(&app, { "aerial_rocks_02.jpg" });
            cyl->move(startX + off, 0.25f, 0.0f);
            cyl->setParallaxMapping(false);

            auto cone = new LCone(&app, { "Rocks011.jpg" });
            cone->move(startX + off * 2, 0.25f, 0.0f);
            cone->setParallaxMapping(false);

            auto sphere = new LSphere(&app, { "Rocks011.jpg" });
            sphere->move(startX + off * 3, 0.25f, 0.0f);
            sphere->setParallaxMapping(false);

            auto ico = new LIcosphere(&app, { "Rocks011.jpg" });
            ico->move(startX + off * 4, 0.25f, 0.0f);
            ico->setParallaxMapping(false);

            auto tor = new LTorus(&app, { "Rocks011.jpg" });
            tor->move(startX + off * 5, 0.25f, 0.0f);
            tor->setParallaxMapping(false);
        }
    }

    const auto drawArrowsOverBefore = [&]() {glDepthFunc(GL_ALWAYS); };
    const auto drawArrowsOverAfter = [&]() {glDepthFunc(GL_LESS); };

    auto xArrow = new LModel(&app, { "arrow.obj" });
    xArrow->move(999999, 999999, 999999);
    xArrow->scale(0.25f, 0.25f, 0.15f);
    xArrow->rotateZ(90.0f);
    xArrow->setBeforeDrawingFunc(drawArrowsOverBefore);
    xArrow->setAfterDrawingFunc(drawArrowsOverAfter);

    auto yArrow = new LModel(&app, { "arrow.obj" });
    yArrow->move(999999, 999999, 999999);
    yArrow->scale(0.25f, 0.25f, 0.15f);
    yArrow->setBeforeDrawingFunc(drawArrowsOverBefore);
    yArrow->setAfterDrawingFunc(drawArrowsOverAfter);

    auto zArrow = new LModel(&app, { "arrow.obj" });
    zArrow->move(999999, 999999, 999999);
    zArrow->rotateX(-90.0f);
    zArrow->scale(0.25f, 0.25f, 0.15f);
    zArrow->setBeforeDrawingFunc(drawArrowsOverBefore);
    zArrow->setAfterDrawingFunc(drawArrowsOverAfter);

    const float scale = 0.0025f;
    auto test = new LModel(&app, "girl.fbx", "Kachujin_diffuse.png", "Kachujin_normal.png", "", "refl_map.jpg");
    //auto test = new LModel(&app, "cube228.obj","Rocks011.jpg","Rocks011.jpg","",
    //    "refl_map.jpg");
    test->scale(scale, scale, scale);
    test->move(0.0f, yOffset, 0.0f);
    test->setParallaxMappingAllMeshes(false);
    test->rotateY(180.0f);
    test->setNormalMappingAllMeshes(false);

    enum class ModificationArrows
    {
        noArrow,
        xArrow,
        yArrow,
        zArrow,
    };

    static ModificationArrows arrow = ModificationArrows::noArrow;

    //test->playAnimation();
    auto prevMousePos = app.getMouseCoords();
    glm::vec2 glPrevMousePos = { prevMousePos.x, app.getWindowSize().y - prevMousePos.y };
    float prevDepth = 1.0f;
    float depth = prevDepth;
    app.setBeforeDrawingFunc([&]()
    {
        if (app.getAppInfo().redactorMode == L_TRUE)
        {
            auto picked = app.getPickedObject();
            if (picked)
            {
                if (arrow != ModificationArrows::noArrow)
                {
                    const float coef = 0.5f;
                    const auto mouseCoords = app.getMouseCoords();
                    const glm::vec2 glMousePos = { mouseCoords.x, app.getWindowSize().y - mouseCoords.y };

                    glm::vec2 normalizedPrevMousePos = app.normalizeMousePos(glPrevMousePos);
                    glm::vec2 normalizedMousePos = app.normalizeMousePos(glMousePos);

                    //auto prevDepth = (float)app.getDepthByPos(glPrevMousePos.x, glPrevMousePos.y) / 255.0f;
                    depth = app.getDepthByPos(glMousePos.x, glMousePos.y); 

                    //if (prevDepth == 0.0f)
                    //    prevDepth = 1.0f;
                    //if (depth == 0.0f)
                    //    depth = 1.0f;

                    glm::vec3 prevMouseInWs = glm::normalize(app.screeSpaceToWorldSpace(normalizedPrevMousePos, prevDepth));
                    glm::vec3 currentMouseInWs = glm::normalize(app.screeSpaceToWorldSpace(normalizedMousePos, depth));

                    interface_.setDepth(depth);
                    interface_.setCursorWorldPos(currentMouseInWs);

                    const glm::vec3 offset = (currentMouseInWs - prevMouseInWs) * coef;

                    const auto mv = picked->getMove();

                    if (arrow == ModificationArrows::xArrow)
                    {
                        LG_DYN_CAST(picked, ->move(mv.x + offset.x, mv.y, mv.z));
                    }

                    else if (arrow == ModificationArrows::yArrow)
                    {
                        LG_DYN_CAST(picked, ->move(mv.x, mv.y + offset.y, mv.z));
                    }

                    else if (arrow == ModificationArrows::zArrow)
                    {
                        LG_DYN_CAST(picked, ->move(mv.x, mv.y, mv.z + offset.z));
                    }
                }

                const auto middleP = picked->getMiddlePoint();
                const auto moveV = picked->getMove();
                xArrow->move({ moveV.x, (middleP.y + moveV.y) / 2.0f, moveV.z });
                yArrow->move({ moveV.x, (middleP.y + moveV.y) / 2.0f, moveV.z });
                zArrow->move({ moveV.x, (middleP.y + moveV.y) / 2.0f, moveV.z });
            }
        }

        static bool firstLaunch = true;
        if (!firstLaunch)
        {
#ifndef NDEBUG
            {
                const auto mouseCoords = app.getMouseCoords();
                const glm::vec2 glMousePos = { mouseCoords.x, app.getWindowSize().y - mouseCoords.y };
                depth = app.getDepthByPos(glMousePos.x, glMousePos.y);

                const glm::vec2 normalizedPrevMousePos = app.normalizeMousePos(glPrevMousePos);
                const glm::vec2 normalizedMousePos = app.normalizeMousePos(glMousePos);

                const glm::vec3 prevMouseInWs = glm::normalize(app.screeSpaceToWorldSpace(normalizedPrevMousePos, prevDepth));
                const glm::vec3 currentMouseInWs = glm::normalize(app.screeSpaceToWorldSpace(normalizedMousePos, depth));
                interface_.setDepth(depth);
                interface_.setCursorWorldPos(currentMouseInWs);
            }
#endif
            prevMousePos = app.getMouseCoords();
            glPrevMousePos = { prevMousePos.x, app.getWindowSize().y - prevMousePos.y };
            prevDepth = (float)app.getDepthByPos(glPrevMousePos.x, glPrevMousePos.y);
        }
        else
            firstLaunch = false;
        if (app.isCursorEnabled())
            return;

        //static auto cMove = c->getMove();
        //const auto move = c->getMove();   

        const float radius = 10.0f;
        const float shiftCoef = -1.05f;
        //glm::vec3 shift = glm::normalize(move - s->getPosition()) * shiftCoef;

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

        GLfloat camX = sin(app.getCurrentFrame());
        GLfloat camZ = cos(app.getCurrentFrame());

        //dirLight->setPosition(glm::vec3(camX * radius, dirLight->getPosition().y,
        //    camZ * radius));

        //if (!app.flag__)
        //    pointLight->setPosition(app.getCameraPos());


        //c->move(cMove.x + radius * camX, c->getMove().y, cMove.z + radius * camZ);

        const float step = 0.01f;
        //c->rotateY(step);

        const float cameraSpeed = 2.0f / ImGui::GetIO().Framerate;
        glm::vec3 cameraPos = app.getCameraPos();
        const auto temp = cameraPos;
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

        cameraPos.y = temp.y;
        app.setCameraPos(cameraPos);
    });

    app.setUserKeyCallback([&](auto w, auto key, auto scancode, auto action, auto mods)
    {
        if (app.isPressed(GLFW_KEY_LEFT_CONTROL))
            app.setCursorEnabling(!app.isCursorEnabled());
        //if (app.isPressed(GLFW_KEY_E))
        //    test->setReflexSize(test->getReflexSize() * 2);
        //if (app.isPressed(GLFW_KEY_Q))
        //    test->setReflexSize(test->getReflexSize() / 2);
        if (app.isPressed(GLFW_KEY_C))
            app.flag__ = !app.flag__;
        if (app.isPressed(GLFW_KEY_P))
            app.pcf = !app.pcf;
        //if (app.isPressed(GLFW_KEY_T))
        //    app.setTesselation(!app.getTesselation());
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
        app.setPitch(app.getAppInfo().api == L_OPENGL ? app.getPitch() - yoffset : app.getPitch() + yoffset);

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
                const auto mouseCoords = app.getMouseCoords();
                const auto obj = app.getObjectByMousePos((size_t)mouseCoords.x, (size_t)app.getWindowSize().y - (size_t)mouseCoords.y);

                if (obj.first != xArrow && obj.first != yArrow && obj.first != zArrow)
                {
                    app.picked = (LShape*)obj.first;
                    interface_.setPickedObject(obj.first);
                }

                if (app.picked)
                {
                    if (obj.first == xArrow)
                        arrow = ModificationArrows::xArrow;
                    else if (obj.first == yArrow)
                        arrow = ModificationArrows::yArrow;
                    else if (obj.first == zArrow)
                        arrow = ModificationArrows::zArrow;
                }

                else
                {
                    xArrow->move(999999, 999999, 999999);
                    yArrow->move(999999, 999999, 999999);
                    zArrow->move(999999, 999999, 999999);
                }
            }

            else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
            {
                arrow = ModificationArrows::noArrow;
            }

        }
    });

    float fov = 45.0f;
    app.setUserScrollCallback([&](GLFWwindow* window, double xOffset, double yOffset)
    {
        //const float min = 1.0f;
        //const float max = 45.0f;
        //const float scale = 1.0f / 25.0f;

        //if (fov >= min && fov <= max)
        //    fov -= yOffset * scale;
        //if (fov < min)
        //    fov = min;
        //if (fov > max)
        //    fov = max;
        //app.setFov(fov);

        //if (fov <= min)
        {
            const auto offset = yOffset / 5.0f;
            const auto cameraPos = app.getCameraPos();
            const auto cameraFront = glm::normalize(app.getCameraFront()) * (float)offset;
            app.setCameraPos({
                cameraPos.x + cameraFront.x,
                cameraPos.y - cameraFront.y,
                cameraPos.z + cameraFront.z });
        }
    });
    //app.setDirLightPos(app.getCameraPos());
    app.loop();
    return 0;
}