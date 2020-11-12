#include "LMessageBox.h"
#include "LMessageBox.h"
#include "LMessageBox.h"
#include "LApp.h"

LGraphics::LMessageBox::LMessageBox(LApp* app, const std::string text, const std::string buttonText, const char* path)
    :LText(app,text,path)
{
    scale(0.45f, 0.35f, 1.0f);
    scaleText(getTextScale()/1.7f);
    auto b1 = new LIButton(app);
    b1->scale(0.1f, 0.1f, 1.0f);
    b1->move(getCenter().x, getBottomLeftCorner().y + 0.07f, 0.0f);
    b1->setWidgetMovability(false);
    b1->setLabel(buttonText);
    b1->color(0, 0, 0);
    b1->setLabelColor(255, 255, 255);
    b1->setClickEventFunction([&]() 
    {
        this->app->deleteWidget(this);
    });
    b1->turnOffTexture();
    bottomBorder = b1->getTopLeftCorner().y - b1->getBottomLeftCorner().y;

    setVerticalScroller(new LVerticalScroller(app));
    alignText();

    vertScroller->moveScrollerToPos(0); 
    button = b1;
    setAnimation([]() {});
    addInnerWidget(b1);
    widgetMovability = true;
}

void LGraphics::LMessageBox::move(const fvect3 val)
{
    move(val.x, val.y, val.z);
}

void LGraphics::LMessageBox::move(const float x, const float y, const float z)
{
    LShape::move(x, y, z);
    LText::move(x, y, z);
    vertScroller->moveScrollerToPos(0);
}
