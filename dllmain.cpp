/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LWRectangle.h"
#include "dllmain.h"
using namespace LGraphics;

extern "C"
{
    void onCreate()
    {
        println("onCreate() func began");
        app = new LApp(10);

        const size_t objCount = app->getObjectsLimit();

        float offsetX = 0.0f;
        for (size_t i = 0; i < objCount; ++i)
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
        println("onCreate() func ended");
    }

    void loop()
    {
        app->loop();
    }
}