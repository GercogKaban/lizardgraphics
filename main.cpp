/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphics
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"
#include "LSymbol.h"

int main()
{   
    LGraphics::LApp app;

    LGraphics::LBaseComponent c;

    auto obj = new LGraphics::LIRectangle(&app, nullptr, "image.png",&c);
    auto b = new LGraphics::LIButton(&app, nullptr, "image.png",&c);

    b->scale({ 0.1f,0.1f,1.0f });
    b->move(100,50);
    b->color(255, 0, 0);

    // show/hide image
    b->setClickEventFunction([&]() {obj->setHidden(!obj->isHidden()); });
    app.loop();
    return 0;
}