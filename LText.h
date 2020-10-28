#pragma once
#include "LIButton.h"
#include "LLine.h"
#include "LScroller.h"

namespace LGraphics
{
    class LText : public LIButton
    {
    public:
        
        friend LVerticalScroller;
        friend LHorizontalScroller;

        LText(LApp* app, const std::string = "", const char* path = nullptr);
        void draw() override;
        void scale(const fvect3 val) override;
        void move(const size_t x, const size_t y) override;

        std::vector<Text> getText() const;

        void setText(const std::string& text);
        void addText(const std::string text);
        void addText(const unsigned int symbol);
        void removeLastSymbol();

        void setVerticalScroller(LVerticalScroller* scroller);
        void setHorizontalScroller(LHorizontalScroller* scroller);

        LVerticalScroller* vertScroller = nullptr;
        LHorizontalScroller* horizScroller = nullptr;

    protected:

        std::vector<Text>::const_iterator begin;
        std::vector<Text>::const_iterator end;

        void initWidget();

        void calculateMaxLength();

        void yAlign();
        void alignText();
        void showFrom(int position);

        bool outOfBordersY(float y);

        void pushNewString();

        void setLabel(const std::string) override {}

        float leftBorder = 0.038f;
        float rightBorder = 0.03f;
        float topBorder = 0.085f;
        float bottomBorder = 0;

        float strIndent = 0.1f;
        std::vector<Text> text;

        size_t hiddenStrings = 0;
        float maxLength = 0.0f;
    };
}

