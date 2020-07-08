#include "LTextEdit.h"
#include "LRectangleBuffer.h"
#include "additional.h"
#include <cassert>

LGraphics::LTextEdit::LTextEdit(LApp * app, const std::string text_, LObject* parent, const char* path, LBaseComponent* component)
    :LIButton(app, parent, path, component)
{
    initWidget();
    addText(text_);
    turnOffColor();
    setAnimation([&]()
    {
        static bool fading = true;
        if (getTransparency() <= 0.25f)
            fading = false;
        else if (getTransparency() >= 0.85f)
            fading = true;
        if (fading)
            transparency(getTransparency() - 0.015f);
        else if (!fading)
            transparency(getTransparency() + 0.015f);
    });
}

void LGraphics::LTextEdit::draw()
{
    if (app->getActiveWidget() == this)
        doAnimation();
    LRectangleShape::draw();
    for (auto str = begin; str < end; str++)
    {
        auto toScreen = glCoordsToScreenCoords(app->getWindowSize(), { str->pos.x, str->pos.y + hiddenStrings*strIndent });
        toScreen.y = app->getWindowSize().y - toScreen.y;
        LText::display(str->text, toScreen.x, toScreen.y, str->scale, str->color);
    }
}

void LGraphics::LTextEdit::scale(const fvect3 val)
{
    scale_ = val;
    alignText();
}

void LGraphics::LTextEdit::move(const size_t x, const size_t y)
{
    auto coords = pointOnScreenToGlCoords(app->getWindowSize(), { (float)x,(float)y });
    move_ = { coords.x,coords.y, 0.0f };
    alignText();
}

std::vector<LGraphics::Text> LGraphics::LTextEdit::getText() const
{
    return text;
}

void LGraphics::LTextEdit::setText(const std::string& text_)
{
    text.clear();
    pushNewString();
    addText(text_);
}

void LGraphics::LTextEdit::addText(const std::string text_)
{
    for (auto& symbol : text_)
        addText(symbol);
}

void LGraphics::LTextEdit::addText(const unsigned int symbol)
{
    auto char_ = LText::characters[symbol];
    if (symbol == '\n')
        pushNewString();

    else if (char_.advance / 64 + text.back().length + rightBorder * app->getWindowSize().x <= maxLength)
    {
        text.back().text += symbol;
        text.back().length += char_.advance / 64;
    }

    else
    {
        pushNewString();
        addText(symbol);
    }
}

void LGraphics::LTextEdit::removeLastSymbol()
{
    if (text.back().text.empty() && text.size() != 1)
    {
        hiddenStrings > 0 ? hiddenStrings-- : hiddenStrings;
        text.pop_back();
        yAlign();
    }

    if (!text.back().text.empty())
    {
        auto char_ = LText::characters[text.back().text.back()];
        text.back().length -= char_.advance / 64;
        text.back().text.pop_back();
    }
}

void LGraphics::LTextEdit::calculateMaxLength()
{
    maxLength = calculateWidgetLength() - rightBorder;
}

void LGraphics::LTextEdit::yAlign()
{
    begin = text.begin() + hiddenStrings;
    end = text.end();
}

void LGraphics::LTextEdit::alignText()
{
    std::string wholeText;
    for (auto& str : text)
        wholeText += str.text;
    text.clear();
    initWidget();
    addText(wholeText);
}

bool LGraphics::LTextEdit::outOfBordersY(float y)
{
    return y < getBottomLeftCorner().y;
}

void LGraphics::LTextEdit::pushNewString()
{
    Text temp;
    if (text.size())
        temp.pos = {text.back().pos.x, text.back().pos.y - strIndent};
    else
    {
        fvect3 topLeftCorner = getTopLeftCorner();
        fvect3 topRightCorner = getTopRightCorner();
        temp.pos = { topLeftCorner.x + leftBorder, topLeftCorner.y - topBorder };
    }

    text.push_back(temp);
    if (outOfBordersY(temp.pos.y))
        hiddenStrings++;
    yAlign();
}

void LGraphics::LTextEdit::initWidget()
{
    hiddenStrings = 0;
    pushNewString();
    calculateMaxLength();
    begin = text.begin();
    end = text.end();
}
