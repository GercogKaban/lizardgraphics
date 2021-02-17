/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#define _NODISCARD 
#include <unordered_map>
#include "LWRectangle.h"
#include "dllmain.h"
#include "LLogger.h"
using namespace LGraphics;

LGraphics::LApp* app;
extern "C"
{
    void onCreate()
    {
        PRINTLN("onCreate() func began");

        LGraphics::LAppCreateInfo info;

        const size_t poolSize = 100;


        info.anisotropy = 16;
        info.projection = L_PERSPECTIVE;
        info.saveObjects = L_FALSE;
        info.loadObjects = L_FALSE;
        info.lighting = L_TRUE;
        info.poolSize = poolSize;
        info.logFlags = ASYNC_LOG | CONSOLE_DEBUG_LOG | FILE_DEBUG_LOG | FILE_RELEASE_LOG;
        app = new LGraphics::LApp(info);

        //const size_t objCount = app->getObjectsLimit();

        float offsetX = 0.0f;
        for (size_t i = 0; i < 1; ++i)
        {
            auto w = new LWRectangle(app);
            w->color(255, 255, 255);
            w->move(0.0f, 0.0f, 0.0f);
            w->bindTexture("arbiter.png");
        }

        app->setImgui([&]()
            {
                ImGui::Text("Hello, world %d", 123);
                ImGui::Text("Hello, worgld %d", 123);
                ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(),
                    ImGui::GetFontSize(), ImVec2(0.5f, 0.5f), ImColor(0, 0, 0, 255), "test", 0, 0.0f, 0);
            });
        PRINTLN("onCreate() func ended");
    }

    void loop()
    {
        app->loop();
    }
}
