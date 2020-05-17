#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"

int main()
{
    LGraphics::LApp app;
    LGraphics::LBaseComponent c;

    auto obj = new LGraphics::LIRectangle(&app, nullptr, "image.png",&c);
    auto b = new LGraphics::LIButton(&app, nullptr, "image.png",&c);

    //auto text = new LGraphics::LSymbol(&app);

    //screen coords
    //text->move(200, 500);
    //text->scale(0.1f,0.2f,0.0f);
    b->scale({ 0.1f,0.1f,1.0f });
    b->move(100,50);
    //text->color(255, 0, 0);

    b->color(255, 0, 0);

    // show/hide image
    b->setClickEventFunction([&]() {obj->setHidden(!obj->isHidden()); });
    app.loop();
    return 0;
}