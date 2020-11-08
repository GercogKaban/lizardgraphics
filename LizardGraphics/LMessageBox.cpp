#include "LMessageBox.h"

LGraphics::LMessageBox::LMessageBox(LApp * app, const std::string text, const std::string buttonText, const char* path)
    :LText(app,text,path)
{
    auto b1 = new LIButton(app, "..path");
    b1->scale(0.1f, 0.1f, 1.0f);
    b1->move(getMove().x, getMove().y+0.05f, 1.0f);
    b1->setWidgetMovability(false);
    b1->setLabel(buttonText);
    b1->turnOffTexture();
    innerWidgets.push_back(b1);
}
