#include "pch.h"
#include "LApp.h"
#include "LText.h"

namespace LGraphics
{
    LGraphics::LText::LText(LApp * app, const std::string text_, const char* path, bool vertScroller)
        :LIButton(app, path)
    {
        if (vertScroller)
            setVerticalScroller(new LVerticalScroller(app));
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

    void LGraphics::LText::draw()
    {
        LRectangleShape::draw();
        for (auto str = begin; str < end; str++)
        {
           if (outOfBordersY(str->pos.y + (vertScroller? vertScroller->currentPos : hiddenStrings) * strIndent))
               continue;
            auto toScreen = glCoordsToScreenCoords(app->getWindowSize(), { str->pos.x, str->pos.y + (vertScroller ? vertScroller->currentPos : hiddenStrings) * strIndent });
            toScreen.y = app->getWindowSize().y - toScreen.y;
            LLine::display(str->text, toScreen.x, toScreen.y, textScale, textColor);
        }
    }

    void LGraphics::LText::scale(const fvect3 val)
    {
        scale_ = val;
        alignText();
        if (vertScroller)
            vertScroller->reloadScroller(hiddenStrings);
    }

    void LText::scale(const float x, const float y, const float z)
    {
        scale(fvect3(x, y, z));
    }

    void LText::scaleText(const float scale)
    {
        textScale = scale;
        auto text = getText();
        setText(text[0]);
        for (size_t i = 1; i < text.size();++i)    
            LText::addText(text[i]);
    }

    void LGraphics::LText::move(const size_t x, const size_t y)
    {
        auto coords = pointOnScreenToGlCoords(fvect2(app->getWindowSize()), { (float)x,(float)y });
        move(coords.x, coords.y, 0.0f);
    }

    void LText::move(float x, float y, float z)
    {
        move_ = { x,y,z};
        if (vertScroller)
            vertScroller->reloadScroller(hiddenStrings);
        alignText();
    }

    void LText::move(const fvect3 val)
    {
        move(val.x, val.y, val.z);
    }

    void LText::setTextColor(const unsigned char r, const unsigned char g, const unsigned char b)
    {
       textColor  = { (float)r / (float)UINT8_MAX, (float)g / UINT8_MAX,(float)b / UINT8_MAX };
    }

    std::vector<std::string> LText::getText() const
    {
        std::vector<std::string> res;
        res.reserve(text.size());
        for (auto& str : text)
            res.push_back(str.text);
        return res;
    }

    void LGraphics::LText::setText(const std::string& text_)
    {
        text.clear();
        initWidget();
        LText::addText(text_);
    }

    void LGraphics::LText::addText(const std::string text_)
    {
        depth++;
        for (auto& symbol : text_)
            LText::addText(symbol);
        depth--;

        if (depth == 0 && textScalling && lastTextScalling)
        {
            textScallingStack.push(lastTextScalling);
            lastTextScalling = 0.0f;
        }
    }

    void LGraphics::LText::addText(const unsigned int symbol)
    {
        addSymb(symbol);
    }

    void LText::addSymb(const unsigned int symbol)
    {
        auto char_ = LLine::characters[symbol];
        if (symbol == '\n')
            pushNewString(false);

        else if ((char_.advance >> 6)*textScale + text.back().length + rightBorder * app->getWindowSize().x <= maxLength)
        {
            text.back().text += (char)symbol;
            text.back().length += (char_.advance >> 6) * textScale;
            if (vertScroller && hiddenStrings > 0)
                vertScroller->reloadScroller(hiddenStrings);
        }

        else
        {
            if (!textScalling)
            {
                if (wordGap)
                    pushNewString(false);
                else
                {
                    bool gap = text.back().text.size()? text.back().text.back() != ' ' : false;
                    pushNewString(gap);
                }
            }
            else if (textScalling)
            {
                lastTextScalling += -getTextScale()*0.05;
                scaleText(getTextScale() - getTextScale()*0.05);
            }
            LText::addSymb(symbol);
        }
    }

    void LGraphics::LText::removeLastSymbol()
    {
        if (text.back().text.empty() && text.size() != 1)
        {
            hiddenStrings > 0 ? hiddenStrings--, vertScroller? vertScroller->reloadScroller(hiddenStrings), vertScroller->moveScrollerToPos(hiddenStrings) :void() : hiddenStrings;
            text.pop_back();
            yAlign();
        }

        if (!text.back().text.empty())
        {
            auto char_ = LLine::characters[text.back().text.back()];
            text.back().length -= (char_.advance >> 6)*textScale;
            text.back().text.pop_back();
            if (textScalling)
            {
                scaleText(getTextScale() - textScallingStack.top());
                textScallingStack.pop();
            }
        }
    }

    void LText::clear()
    {
        setText("");
    }

    void LGraphics::LText::setVerticalScroller(LVerticalScroller* scroller)
    {
        if (vertScroller)
        {
            auto obj = app->getInterfaceObjects();
            for (size_t i = 0; i < obj.size(); ++i)
                if (obj[i] == vertScroller)
                    obj.erase(obj.begin() + i);
            delete vertScroller;
        }
        scroller->setParent(this);
        vertScroller = scroller;
        scroller->scaleWithoutAlign({ scale_.x / 25.0f,scale_.y, scale_.z });
        //scroller->move(getTopRightCorner());
        //scroller->move(fvect3(getTopRightCorner().x, move_.y, move_.z));
        scroller->reloadScroller(hiddenStrings);
        addInnerWidget(scroller);
        //innerWidgets.push_back(scroller);
    }

    void LGraphics::LText::setHorizontalScroller(LHorizontalScroller * scroller)
    {
        scroller->setParent(this);
        if (horizScroller && innerWidgets)
            std::remove(innerWidgets->begin(), innerWidgets->end(), horizScroller);
        horizScroller = scroller;
        scroller->scale({ scale_.x, scale_.y / 25.0f, scale_.z });
        //scroller->move(fvect3(move_.x, move_.y + (getTopLeftCorner().x + getBottomLeftCorner().y) / 2.0f, move_.z));
        scroller->move(fvect3(move_.x, getTopLeftCorner().y, move_.z));
        if (!innerWidgets) innerWidgets = new std::vector<LGraphics::LWidget*>();
        innerWidgets->push_back(scroller);
    }

    void LGraphics::LText::calculateMaxLength()
    {
        maxLength = calculateWidgetLength() - rightBorder;
    }

    void LGraphics::LText::yAlign()
    {
        begin = text.begin() + hiddenStrings;
        end = text.end();
    }

    void LGraphics::LText::alignText()
    {
        std::string wholeText;
        for (size_t i = 0; i < text.size(); ++i)
            wholeText += text[i].text += '\n';
        wholeText.pop_back();
        text.clear();
        initWidget();
        LText::addText(wholeText);
    }


    void LText::showFrom(int position)
    {
        begin = text.begin() + position;
    }

    bool LGraphics::LText::outOfBordersY(float y)
    {
        return y < getBottomLeftCorner().y + bottomBorder;
    }

    void LGraphics::LText::pushNewString(bool gap)
    {
        fvect3 topLeftCorner = getTopLeftCorner();
        Text temp;
        if (gap)
        {
            for (auto it = text.back().text.rbegin(); it != text.back().text.rend(); it++)
            {
                if (*it == ' ')
                    break;
                temp.text.insert(temp.text.begin(),*it);
                text.back().text.pop_back();
            }
        }

        if (text.size())
            temp.pos = { topLeftCorner.x + leftBorder, text.back().pos.y - strIndent };
        else
            temp.pos = { topLeftCorner.x + leftBorder, topLeftCorner.y - topBorder };

        text.push_back(temp);
        if (outOfBordersY(temp.pos.y))
            hiddenStrings++;

        if (vertScroller && hiddenStrings > 0)
            vertScroller->reloadScroller(hiddenStrings);
        yAlign();
    }

    void LGraphics::LText::initWidget()
    {
        hiddenStrings = 0;
        pushNewString(false);
        calculateMaxLength();
        yAlign();
    }

}