﻿/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"
#include "LTextEdit.h"
#include "additional.h"

using namespace LGraphics;

int main()
{   
    LGraphics::LApp app(1920,1080);

    LGraphics::LBaseComponent c;

    auto back = new LGraphics::LIRectangle(&app, nullptr, "brge_back.png", &c);
    back->scale(2.0f, 2.0f, 1.0f);
    back->turnOffColor();

    auto e = new LIButton(&app, nullptr, "image.png",&c);
    auto d = new LIButton(&app, nullptr, "image.png", &c);

    auto textEdit = new LTextEdit(&app, "Hello, world!", nullptr, "test.png", &c);
    textEdit->turnOffTexture();
    textEdit->color(255, 255, 255);
    //textEdit->move(app.getWindowSize().x/2, app.getWindowSize().y / 2);
    textEdit->setVerticalScroller(new LGraphics::LVerticalScroller(&app, nullptr, "", &c));
    //textEdit->setHorizontalScroller(new LGraphics::LHorizontalScroller(&app, nullptr, "", &c));
    //textEdit->scale({ 1.5f,1.8f,1.0f });
    //textEdit->transparency(0.85f);
    //textEdit->move(700, 500);

    e->scale({ 1.9f,1.8f,1.0f });
    //e->move(100,50);
    e->color(255, 0, 0);

    d->scale({ 0.1f,0.1f,1.0f });
    d->move(700, 50);
    d->color(0, 0, 0);
    d->setLabel("aue");
    d->setLabelColor(255, 255, 255);
    d->turnOffTexture();

    e->setClickEventFunction([&]() {textEdit->scale({ 1.5f,0.5f,1.0f }); });
    d->setClickEventFunction([&]() {textEdit->scale({ 0.5f,0.5f,1.0f }); });

    app.loop();
    return 0;
}