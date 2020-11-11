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

#include "CodeGen.h"
//#include "testFile.hxx"

using namespace LGraphics;

int main()
{
    LGraphics::LApp app;

    //genWidgets(&app);
    CodeGen gen;

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

    //auto b3 = new LIButton(&app, "test.png");
    //b3->move(0.694633, -0.016667, 0.000000);
    //b3->setLabel("LTextEdit");
    //b3->turnOffTexture();
    //b3->scale(0.5f, 0.1f, 1.0f);
    //b3->setWidgetMovability(false);
    //b3->setClickEventFunction(([&]()
    //{
    //    auto obj = new LTextEdit(&app, "","path...");
    //    obj->turnOffTexture();
    //}));
    //gen.notGenerateData(b3);

    auto b3 = new LWRectangle(&app, "image11.bmp");
    b3->setWidgetMovability(false);
    //b3->setName("tile");
    //b3->setIsometricView();
    //LGraphics::LCounterF* c1, *c2, *c3;

    //auto func = [&]()
    //{
    //    glm::mat4 rot(1.0f);
    //    rot = glm::rotate(rot, glm::radians((float)c1->getNum()), { 1.0f,0.0f,0.0f });
    //    rot = glm::rotate(rot, glm::radians((float)c2->getNum()), { 0.0f,1.0f,0.0f });
    //    rot = glm::rotate(rot, glm::radians((float)c3->getNum()), { 0.0f,0.0f,1.0f });
    //    b3->setRotate(rot);
    //};

    //c1 = new LGraphics::LCounterF(&app, "", "..");
    //c1->move(-0.6f, 0.9f, 1.0f);
    //c1->setChangingEventFunc(func);
    //c1->setWidgetMovability(false);
    //c2 = new LGraphics::LCounterF(&app, "", "..");
    //c2->move(-0.3f, 0.9f, 1.0f);
    //c2->setChangingEventFunc(func);
    //c2->setWidgetMovability(false);
    //c3 = new LGraphics::LCounterF(&app, "", "..");
    //c3->move(-0.0f, 0.9f, 1.0f);
    //c3->setChangingEventFunc(func);
    //c3->setWidgetMovability(false);

    //auto edit = new LGraphics::LTextEdit(&app);
    //edit->setVerticalScroller(new LVerticalScroller(&app));
    //auto msg = new LGraphics::LMessageBox(&app,"Error 228");
    //msg->setVerticalScroller(new LVerticalScroller(&app));
    //msg->setTextColor(255, 0, 0);
    //msg->color(0, 0, 0);

    auto msg = new LMessageBox(&app, "ERROR 228ggg tregre gerger ggfewweg gwregwerg gwergewrg grwegewr g");

    app.setSleepTime(5);
    app.setWidgetsMovability(true);
    app.loop();
    gen.generateCode("testFile.hxx", &app, "app");
    return 0;
}