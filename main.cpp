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
#include "LList.h"

#include "additional.h"

#include "CodeGen.h"
//#include "testFile.hxx"

using namespace LGraphics;

int main()
{
    LGraphics::LApp app;
    auto t = new Text("Test", { 500,500 }, 1.0f, { 255,0,0 });
    app.addText(t);
    //auto length = LTextRender::getTextLength(t);

    //auto textLength = LTextRender::getTextLength(*t);
    //auto rect = new LIRectangle(&app);
    //auto pos = pointOnScreenToGlCoords(app.getWindowSize(), { 500,500 });
    //rect->move(fvect3(pos.x, pos.y, 0.0f));
    //rect->scale(textLength / (float)(app.getWindowSize().x / 4), (float)48 / (float)(app.getWindowSize().y / 4), 1.0f);

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
    //    auto obj = new LIRectangle(&app);
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
    //LGraphics::LCounterF* c1, *c2, *c3;

    /*auto list = new LList(&app);
    list->color(255, 255, 255);
    list->setLabel("Widgets");
    list->scale(0.1f, 0.1f, 1.0f);

    list->setVertOrientation(LList::bottom);
    list->setHorizOrientation(LList::right);

    auto b1 = new LIButton(&app);
    b1->color(255, 255, 255);
    b1->setClickEventFunction([&]() {new LIButton(&app); });
    auto b2 = new LIButton(&app);
    b2->color(255, 255, 255);
    b2->setClickEventFunction([&]() {new LList(&app); });
    auto b3 = new LIButton(&app);
    b3->color(255, 255, 255);
    b3->setClickEventFunction([&]() {new LCounterF(&app); });
    auto b4 = new LIButton(&app);
    b4->color(255, 255, 255);
    b4->setClickEventFunction([&]() {new LIRectangle(&app); });
    auto b5 = new LIButton(&app);
    b5->color(255, 255, 255);
    b5->setClickEventFunction([&]() {new LText(&app); });
    auto b6 = new LIButton(&app);
    b6->color(255, 255, 255);
    b6->setClickEventFunction([&]() {new LTextEdit(&app); });
    auto b7 = new LIButton(&app);
    b7->color(255, 255, 255);
    b7->setClickEventFunction([&]() {new LFlag(&app); });
    auto b8 = new LIButton(&app);
    b8->color(255, 255, 255);
    b8->setClickEventFunction([&]() {new LColorBar(&app); });

    list->addInnerWidget(b1);
    list->addInnerWidget(b2);
    list->addInnerWidget(b3);
    list->addInnerWidget(b4);
    list->addInnerWidget(b5);
    list->addInnerWidget(b6);
    list->addInnerWidget(b7);
    list->addInnerWidget(b8);

    b1->setLabel("LIButton");
    b2->setLabel("LList");
    b3->setLabel("LCounterF");
    b4->setLabel("LIRectangle");
    b5->setLabel("LText");
    b6->setLabel("LTextEdit");
    b7->setLabel("LFlag");
    b8->setLabel("LColorBar");*/

    //auto bar = new LColorBar(&app);
    //bar->move(500, 60);
    //bar->setFullness(0.0f);



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

    app.setSleepTime(5);
    app.setWidgetsMovability(true);
    app.loop();
    //gen.generateCode("testFile.hxx", &app, "app");
    return 0;
}