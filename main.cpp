/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LTextEdit.h"
#include "LWRectangle.h"
#include "LFlag.h"
#include "LCounter.h"
#include "LMessageBox.h"
#include "LColorBar.h"

#include "additional.h"

#include "CodeGen.h"
//#include "testFile.hxx"

using namespace LGraphics;

int main()
{
    LGraphics::LApp app;

    //genWidgets(&app);
    //CodeGen gen;

    //auto b1 = new LIButton(&app, "test.png");
    //b1->move(0.693590, -0.394444, 0.000000);
    //b1->setLabel("LIButton");
    //b1->turnOffTexture();
    //b1->scale(0.5f, 0.1f, 1.0f);
    //b1->setWidgetMovability(false);
    //b1->setClickEventFunction(([&]() 
    //{
    //    auto obj = new LIButton(&app, "path...");
    //}));
    //gen.notGenerateData(b1);

    //auto b2 = new LIButton(&app, "test.png");
    //b2->move(0.694633, -0.214815, 0.000000);
    //b2->setLabel("LIRectangle");
    //b2->turnOffTexture();
    //b2->scale(0.5f, 0.1f, 1.0f);
    //b2->setWidgetMovability(false);
    //b2->setClickEventFunction(([&]()
    //{
    //    auto obj = new LIRectangle(&app, "path...");
    //}));
    //gen.notGenerateData(b2);

    //auto r = new LWRectangle(&app, "image11.bmp");
    //r->setMatrices();
    //r->scale(0.2f, 0.2f,1.0f);
    //r->setWidgetMovability(false);

    //auto b = new LIButton(&app);
    //b->scale(0.1f, 0.1f, 1.0f);
    //b->move(-0.5f, -0.5f, 0.0f);

    //b->setClickEventFunction([&]()
    //{
    //    auto scale = r->getScale();
    //    auto move = r->getMove();
    //    r->scale(scale.x + 0.05f, scale.y, scale.z);
    //    r->move(move.x + 0.05f/2, move.y, move.z);
    //});


    //auto edit = new LGraphics::LTextEdit(&app);

    //b3->setName("tile");
    //b3->setIsometricView();
    LGraphics::LCounterF* c1, *c2, *c3;

    //auto func = [&]()
    //{
    //    r->setRotate(fvect3{ (float)c1->getNum(), (float)c2->getNum(), (float)c3->getNum() });
    //};

    //c1 = new LGraphics::LCounterF(&app, "");
    //c1->move(-0.6f, 0.9f, 1.0f);
    //c1->setChangingEventFunc(func);
    //c1->setWidgetMovability(false);
    //c2 = new LGraphics::LCounterF(&app, "");
    //c2->move(-0.3f, 0.9f, 1.0f);
    //c2->setChangingEventFunc(func);
    //c2->setWidgetMovability(false);
    //c3 = new LGraphics::LCounterF(&app, "");
    //c3->move(-0.0f, 0.9f, 1.0f);
    //c3->setChangingEventFunc(func);
    //c3->setWidgetMovability(false);

    //auto edit = new LGraphics::LTextEdit(&app);
    //edit->setVerticalScroller(new LVerticalScroller(&app));
    //auto msg = new LGraphics::LMessageBox(&app,"Error 228");
    //msg->setVerticalScroller(new LVerticalScroller(&app));
    //msg->setTextColor(255, 0, 0);
    //msg->color(0, 0, 0);

    //auto colorBar = new LColorBar(&app);
    //colorBar->setFullness(1.0f);
    //auto msg = new LMessageBox(&app, "gfreo[gker[ok gker[ogkero[g gker[ogkre[o kgkereo");
    
    //auto test = new LWRectangle(&app);
    //test->move(-0.5f,0.5f,0.0f);
    //auto coords = test->getScreenCoords();
    //auto screenCoords = glCoordsToScreenCoords(app.getWindowSize(), fvect2(coords.x,coords.y));

    auto test = app.lwRectPool.pop();
    test->color(255, 0, 0);

    app.lwRectPool.addToPool(test);

    auto test1 = app.lwRectPool.pop();

    app.removeWidget(test);

    app.setSleepTime(5);
    app.setWidgetsMovability(true);
    app.loop();
    //gen.generateCode("testFile.hxx", &app, "app");
    return 0;
}