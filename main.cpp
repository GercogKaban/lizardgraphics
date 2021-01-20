﻿/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LWRectangle.h"
using namespace LGraphics;

int main()
{
    LGraphics::LApp app(5);

    //app.setSleepTime(5);
    const size_t objCount = app.getObjectsLimit();

    //float offsetX = 0.0f;
    //for (size_t i = 0; i < objCount; ++i)
    //{
        auto w = new LWRectangle(&app, "arbiter.png");
        w->color(255, 255, 255);
        w->scale(0.3f, 0.6f, 1.0f);
        w->move(0.0f, 0.0f, 0.0f);
        w->bindTexture("image1.bmp");
       // offsetX -= 0.001;

        auto w1 = new LWRectangle(&app);
        w1->color(255, 255, 255);
        w1->scale(0.3f, 0.6f, 1.0f);
        w1->move(-0.5f, 0.5f, 0.0f);
        w1->bindTexture("arbiter.png");
        
    //}

    app.loop();
    return 0;
}