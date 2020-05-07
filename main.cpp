#include "LizardGraphics/LizardGraphics.h"

int main()
{
    LGraphics::LApp app;

    auto obj = new LGraphics::LIRectangle(&app,nullptr, "image.png");
    auto b = new LGraphics::LIButton(&app,nullptr, "image.png");
    auto text = new LGraphics::LText(&app);

    //screen coords
    text->move(200, 500);
    text->scale(1.0f,0.5f,0.0f);
    b->scale({ 0.1f,0.1f,1.0f });
    b->move(100,50);

    b->color(255, 0, 0);

    // show/hide image
    b->setClickEventFunction([&]() {obj->setHidden(!obj->isHidden()); });
    app.loop();
    return 0;
}