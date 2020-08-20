﻿/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LTextEdit.h"
#include "additional.h"
#include <cmath>

using namespace LGraphics;

int main()
{   
    LGraphics::LApp app;

    //auto e = new LIButton(&app, "image.png");
    auto d = new LIButton(&app, "image.png");

    std::string test(1, 'i');
    auto textEdit = new LTextEdit(&app, test, "test.png");
    textEdit->turnOffTexture();
    textEdit->color(255, 255, 255);
    textEdit->move(app.getWindowSize().x/2, app.getWindowSize().y / 2);
    textEdit->setVerticalScroller(new LGraphics::LVerticalScroller(&app));

    //textEdit->scale({0.5f ,0.5f,1.0f });
    textEdit->transparency(0.85f);
    textEdit->move(700, 500);
    //e->scale({ 0.1f,0.1f,1.0f });
    //e->move(100,50);
    //e->color(255, 0, 0);

    d->scale(textEdit->vertScroller->getScale());

    //d->move(700,200);
    d->color(0, 0, 0);
    //d->setLabel("aue");
    d->setLabelColor(255, 255, 255);
    d->turnOffTexture();

    //e->setClickEventFunction([&]() {textEdit->scale({ 1.5f,0.5f,1.0f }); });
    d->setClickEventFunction([&]() {textEdit->move(app.getWindowSize().x / 3, app.getWindowSize().y / 2); });
    textEdit->move(800, 500);
    app.loop();
    return 0;
}