/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LTextEdit.h"
#include "LWRectangle.h"

using namespace LGraphics;

int main()
{
    LGraphics::LApp app;

    auto t1 = "test1.png";
    auto t2 = "test2.png";
    //auto e = new LIButton(&app, "image.png");
    auto d = new LIButton(&app, t1,false);
    //auto unit = new LWRectangle(&app,t1);

    //std::string test("test\ntest");
    //auto textEdit = new LTextEdit(&app, test);
    //textEdit->turnOffTexture();
    //textEdit->color(255, 255, 255);
    //textEdit->move(app.getWindowSize().x / 2, app.getWindowSize().y / 2);
    //textEdit->setVerticalScroller(new LGraphics::LVerticalScroller(&app));

    ////textEdit->scale({0.5f ,0.5f,1.0f });
    //textEdit->transparency(0.85f);
    //textEdit->move(700, 500);
    ////e->scale({ 0.1f,0.1f,1.0f });
    ////e->move(100,50);
    ////e->color(255, 0, 0);

    ////d->move(700,200);
    //d->color(255, 0, 0);
    //d->setLabelColor(255, 255, 255);
    //d->turnOffTexture();

    ////e->setClickEventFunction([&]() {textEdit->scale({ 1.5f,0.5f,1.0f }); });
    d->setClickEventFunction([&]() 
    {
        auto t = new LIButton(&app, t1,false);
        t->move(fvect3{ 0.1f,0.1f,0.1f });
    });
    //textEdit->move(800, 500);
    //textEdit->scale({ 1, 1, 1});
    app.loop();
    return 0;
}