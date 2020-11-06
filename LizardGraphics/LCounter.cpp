#include "LCounter.h"
#include "textures.h"

LGraphics::LCounter::LCounter(LApp * app, const std::string str, const char * path)
    :LTextEdit(app,str, path)
{
    scale(0.2f, 0.1f, 1.0f);
    turnOffTexture();
    setAnimation([](){});

    auto b1 = new LIButton(app, arrow_up, arrow_upSize);
    b1->scale(0.025f, 0.03f, 1.0f);
    b1->move(getMove() + fvect3(0.085f, 0.02f, 0.0f));
    b1->turnOnTexture();
    b1->setWidgetMovability(false);
    b1->setClickEventFunction([&]() 
    {
        if (mode == Integer)
        {
            counterInt += countInterval;
            setText(std::to_string(counterInt));
        }
        else if (mode == Float)
        {
            counterFl += countInterval;
            setText(to_string_with_precision(counterFl, floatPrecision));
        }       
    });
    innerWidgets.push_back(b1);

    auto b2 = new LIButton(app, arrow_down, arrow_downSize);
    b2->scale(0.025f, 0.03f, 1.0f);
    b2->turnOnTexture();
    b2->move(getMove() + fvect3(0.085f, -0.02f, 0.0f));
    b2->setWidgetMovability(false);
    b2->setClickEventFunction([&]()
    {
        if (mode == Integer)
        {
            counterInt -= countInterval;
            setText(std::to_string(counterInt));
        }
        else if (mode == Float)
        {
            counterFl -= countInterval;
            setText(to_string_with_precision(counterFl, floatPrecision));
        }
    });
    innerWidgets.push_back(b2);

    textScalling = true;
    
    setCountInterval(0.5f);
    setFloatPrecision(2);
    setMode(Float);
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
    LShape::scale(x, y, z);
    LText::scale(fvect3(x, y, z));
}

void LGraphics::LCounter::addText(const unsigned int symbol)
{
    if (symbol == '-')
    {
        if (mode == Integer)
        {
            counterInt = -counterInt;
            setText(std::to_string(counterInt));
        }

        else if (mode == Float)
        {
            counterFl = -counterFl;
            setText(to_string_with_precision(counterFl, floatPrecision));
        }
    }

    else if (!isdigit(symbol))
        return;

    else
    {
        if (mode == Integer)
        {
            counterInt = 10 * counterInt + std::stoi(std::string(1, symbol));
            setText(std::to_string(counterInt));
        }

        else if (mode == Float)
        {
            counterFl = 10 * counterFl + std::stoi(std::string(1, symbol));
            setText(to_string_with_precision(counterFl, floatPrecision));
        }
    }
}

void LGraphics::LCounter::addText(const std::string text_)
{
    for (auto& symbol : text_)
        addText(symbol);
}

void LGraphics::LCounter::removeLastSymbol()
{
    if (mode == Integer)
    {
        counterInt /= 10;
        setText(std::to_string(counterInt));
    }

    else if (mode == Float)
    {
        counterFl = (int)counterFl / 10;
        setText(to_string_with_precision(counterFl, floatPrecision));
    }

    if (textScallingStack.size())
    {
        scaleText(getTextScale() - textScallingStack.top());
        textScallingStack.pop();
    }
}

void LGraphics::LCounter::setMode(int mode)
{
    this->mode = mode;
    if (mode == Integer)
    {
        counterInt = 0;
        setText(std::to_string(counterInt));
    }
    else if (mode == Float)
    {
        counterFl = 0.0f;
        setText(to_string_with_precision(counterFl, floatPrecision));
    }
}

void LGraphics::LCounter::clear()
{
    LText::clear();
    counterFl = 0.0f;
    counterInt = 0;
}
