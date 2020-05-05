#include "LizardGraphics/LizardGraphics.h"

int main()
{
    LGraphics::LApp app;
    app.addObject(new LGraphics::LIRectangle(nullptr,"image.png"));
    app.loop();
    return 0;
}