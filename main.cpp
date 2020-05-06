#include "LizardGraphics/LizardGraphics.h"

int main()
{
    LGraphics::LApp app;

    auto obj = new LGraphics::LIRectangle(nullptr, "image.png");
    auto b = new LGraphics::LIButton(nullptr, "image.png");

    b->setScale({ 0.1f,0.1f,1.0f });
    b->move({ -0.9f, 0.9f,0.0f });
    b->setColor(255, 0, 0);

    // show/hide image
    b->setClickEventFunction([&]() {obj->setHidden(!obj->isHidden()); });

    app.addObject(b);
    app.addObject(obj);
    app.loop();
    return 0;
}