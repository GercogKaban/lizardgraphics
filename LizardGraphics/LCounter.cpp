#include "LCounter.h"
#include "LRectangleShape.h"
#include "textures.h"
#include "additional.h"
#include "LApp.h"

LGraphics::LCounter::LCounter(LApp * app, const std::string str, const char * path)
    :LTextEdit(app,str, path,false)
{
    scale(0.2f, 0.1f, 1.0f);
    turnOffTexture();
    setAnimation([](){});

    auto b1 = new LIButton(app, arrow_up, arrow_upSize);
    b1->scale(0.025f, 0.03f, 1.0f);
    b1->move(getTopRightCorner() - fvect3(0.015f, scale_.y / 5, 0.0f));
    b1->turnOnTexture();
    b1->setWidgetMovability(false);
    b1->setName("up");
    innerWidgets.push_back(b1);

    auto b2 = new LIButton(app, arrow_down, arrow_downSize);
    b2->scale(0.025f, 0.03f, 1.0f);
    b2->turnOnTexture();
    b2->move(getTopRightCorner() - fvect3(0.015f, scale_.y / 1.25f, 0.0f));
    b2->setWidgetMovability(false);
    b2->setName("down");
    innerWidgets.push_back(b2);
    textScalling = true;

    calculateMaxLength();
    setOutPrecision(2);
}

void LGraphics::LCounter::calculateMaxLength()
{
    auto arrowUp = (LIButton*)getInnerWidgetByName("up");
    auto arrowXSize = arrowUp ? xGlCoordToScreenCoord(app->getWindowSize(), arrowUp->getTopRightCorner().x) - 
        xGlCoordToScreenCoord(app->getWindowSize(), arrowUp->getTopLeftCorner().x) : 0;
    maxLength = calculateWidgetLength() - rightBorder - arrowXSize;
}

void LGraphics::LCounter::move(const fvect3 val)
{
    move(val.x, val.y, val.z);
}

void LGraphics::LCounter::move(const float x, const float y, const float z)
{
    LShape::move(x, y, z);
    LText::move(x, y, z);
}

void LGraphics::LCounter::scale(const fvect3 val)
{
    scale(val.x, val.y, val.z);
}

void LGraphics::LCounter::scale(const float x, const float y, const float z)
{
    scaleWithoutAlign({ x,y,z });
    if (auto up = getInnerWidgetByName("up")) up->move(getTopRightCorner() - fvect3(0.015f, scale_.y / 5, 0.0f));
    if (auto down = getInnerWidgetByName("down")) down->move(getTopRightCorner() - fvect3(0.015f, scale_.y / 5, 0.0f));

    LText::scale(fvect3(x, y, z));
}

void LGraphics::LCounter::addText(const std::string text_)
{
    for (auto& symbol : text_)
        addText(symbol);
}

LGraphics::LCounterI::LCounterI(LApp * app, const std::string str, const char * path)
    :LCounter(app,str,path)
{
    if (auto up = getInnerWidgetByName("up"))
        ((LIButton*)up)->setClickEventFunction([&]()
    {
        counter += countInterval;
        setText(std::to_string(counter));
    });
    if (auto down = getInnerWidgetByName("down"))
        ((LIButton*)down)->setClickEventFunction([&]()
    {
        counter -= countInterval;
        setText(std::to_string(counter));
    });
    setText(std::to_string(counter));
}

void LGraphics::LCounterI::addNumber(const double val)
{
    counter += val;
    setText(std::to_string(counter));
    changingEventFunc();
}

void LGraphics::LCounterI::setCountInterval(double interval)
{
    countInterval = (int)interval;
}

void LGraphics::LCounterI::addText(const unsigned int symbol)
{
    if (symbol == '-')
    {
        counter = -counter;
        setText(std::to_string(counter));
    }
        
    else if (!isdigit(symbol))
         return;       
    else
    {
       counter = 10 * counter + std::stoi(std::string(1, symbol));
       setText(std::to_string(counter));
    }
    changingEventFunc();
}

void LGraphics::LCounterI::removeLastSymbol()
{
    counter /= 10;
    setText(std::to_string(counter));

    if (textScallingStack.size())
    {
        scaleText(getTextScale() - textScallingStack.top());
        textScallingStack.pop();
    }
    changingEventFunc();
}

void LGraphics::LCounterI::clear()
{
    counter = 0;
    setText(std::to_string(counter));
    changingEventFunc();
}

double LGraphics::LCounterI::getNum() const
{
    return counter;
}

LGraphics::LCounterF::LCounterF(LApp * app, const std::string str, const char * path)
    :LCounter(app, str, path)
{
    LShape::scale(getScale().x + getScale().x*0.1f, getScale().y, getScale().z);

    if (auto up = getInnerWidgetByName("up"))
        ((LIButton*)up)->setClickEventFunction([&]()
    {
        counter += countInterval;
        setText(to_string_with_precision(counter, outPrecision));
    });
    if (auto down = getInnerWidgetByName("down"))
        ((LIButton*)down)->setClickEventFunction([&]()
    {
        counter -= countInterval;
        setText(to_string_with_precision(counter, outPrecision));
    });

    setText(to_string_with_precision(counter,outPrecision));
}

void LGraphics::LCounterF::addNumber(const double val)
{
    counter += val;
    setText(to_string_with_precision(counter, outPrecision));
    changingEventFunc();
}

void LGraphics::LCounterF::setCountInterval(double interval)
{
    countInterval = interval;
}

void LGraphics::LCounterF::addText(const unsigned int symbol)
{
    if (symbol == '-')
    {
        counter = -counter;
        setText(to_string_with_precision(counter, outPrecision));
    }
    else if (!isdigit(symbol))
        return;
    else
    {
        counter = 10 * counter + std::stoi(std::string(1, symbol));
        setText(to_string_with_precision(counter, outPrecision));
    }
    changingEventFunc();
}

void LGraphics::LCounterF::removeLastSymbol()
{
    counter = (int)counter / 10;
    setText(to_string_with_precision(counter, outPrecision));

    if (textScallingStack.size())
    {
        scaleText(getTextScale() - textScallingStack.top());
        textScallingStack.pop();
    }
    changingEventFunc();
}

void LGraphics::LCounterF::clear()
{
    counter = 0.0f;
    setText(to_string_with_precision(counter, outPrecision));
    changingEventFunc();
}

double LGraphics::LCounterF::getNum() const
{
    return counter;
}

LGraphics::LCounterLL::LCounterLL(LApp * app, const std::string str, const char * path)
    :LCounter(app, str, path)
{
    LShape::scale(fvect3(getScale().x*2, getScale().y, getScale().z));

    if (auto up = getInnerWidgetByName("up"))
        ((LIButton*)up)->setClickEventFunction([&]()
    {
        counter += countInterval;
        setText(std::to_string(counter));
    });
    if (auto down = getInnerWidgetByName("down"))
        ((LIButton*)down)->setClickEventFunction([&]()
    {
        counter -= countInterval;
        setText(std::to_string(counter));
    });
    setText(std::to_string(counter));
}

void LGraphics::LCounterLL::addNumber(const double val)
{
    counter += val;
    setText(std::to_string(counter));
    changingEventFunc();
}

void LGraphics::LCounterLL::setCountInterval(double interval)
{
    countInterval = interval;
}

void LGraphics::LCounterLL::addText(const unsigned int symbol)
{
    if (symbol == '-')
    {
        counter = -counter;
        setText(std::to_string(counter));
    }
    else if (!isdigit(symbol))
        return;
    else
    {
        counter = 10 * counter + std::stoll(std::string(1, symbol));
        setText(std::to_string(counter));
    }
    changingEventFunc();
}

void LGraphics::LCounterLL::removeLastSymbol()
{
    counter /= 10;
    setText(std::to_string(counter));

    if (textScallingStack.size())
    {
        scaleText(getTextScale() - textScallingStack.top());
        textScallingStack.pop();
    }
    changingEventFunc();
}

void LGraphics::LCounterLL::clear()
{
    counter = 0;
    setText(std::to_string(counter));
    changingEventFunc();
}

double LGraphics::LCounterLL::getNum() const
{
    return counter;
}
