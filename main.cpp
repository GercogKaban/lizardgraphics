/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphics
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

    auto e = new LIButton(&app, nullptr, "image.png",&c);
    auto d = new LIButton(&app, nullptr, "image.png", &c);

    auto textEdit = new LTextEdit(&app, "Hello, world!", nullptr, "test.png", &c);
    textEdit->move(app.getWindowSize().x/2, app.getWindowSize().y / 2);
    textEdit->scale({ 1.5f,0.5f,1.0f });
    textEdit->transparency(0.85f);

    e->scale({ 0.1f,0.1f,1.0f });
    e->move(100,50);
    e->color(255, 0, 0);

    d->scale({ 0.1f,0.1f,1.0f });
    d->move(250, 50);
    d->color(0, 0, 255);

    e->setClickEventFunction([&]() {textEdit->scale({ 1.5f,0.5f,1.0f }); });
    d->setClickEventFunction([&]() {textEdit->scale({ 0.5f,0.5f,1.0f }); });

    app.loop();
    return 0;
}