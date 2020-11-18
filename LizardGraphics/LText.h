#pragma once
#include <stack>

#include "LIButton.h"
#include "LTextRender.h"
#include "LScroller.h"

namespace LGraphics
{
    class LText : public LIButton
    {
    public:
        
        const char* getObjectType() override { return "LText"; }

        friend LVerticalScroller;
        friend LHorizontalScroller;

        LText(LApp* app, const std::string = "", const char* path = nullptr, bool vertScroller = false);
        void draw() override;

        void scale(const fvect3 val) override;
        void scale(const float x, const float y, const float z) override;

        void scaleText(const float scale);
        float getTextScale() const{ return textScale;}

        void move(const size_t x, const size_t y) override;
        void move(const float x, const float y, const float z) override;
        void move(const fvect3 val) override;

        void setTextColor(const unsigned char r, const unsigned char g, const unsigned char b);

        void setLabel(const std::string text) override { setText(text); }

        //std::vector<Text> getText() const;
        std::vector<std::string> getText() const;

        virtual void setText(const std::string& text);
        virtual void addText(const std::string text);

        virtual void removeLastSymbol();

        virtual void clear();

        void setVerticalScroller(LVerticalScroller* scroller);
        void setHorizontalScroller(LHorizontalScroller* scroller);

        LVerticalScroller* vertScroller = nullptr;
        LHorizontalScroller* horizScroller = nullptr;

    protected:

        virtual void addText(const unsigned int symbol);
        void addSymb(const unsigned int symbol);

        std::vector<Text>::const_iterator begin;
        std::vector<Text>::const_iterator end;

        virtual void alignText();
        virtual void calculateMaxLength();

        void initWidget();

        void yAlign();
        void showFrom(int position);

        bool textScalling = false;
        std::stack<float> textScallingStack;
        float lastTextScalling = 0.0f;

        //bool outOfBordersX(float x);
        bool outOfBordersY(float y);

        void pushNewString(bool gap);

        //void setLabel(const std::string) override {}

        float leftBorder = 0.038f;
        float rightBorder = 0.03f;
        float topBorder = 0.085f;
        float bottomBorder = 0;

        float strIndent = 0.1f;
        std::vector<Text> text;

        size_t hiddenStrings = 0;
        float maxLength = 0.0f;

        float deFaultTextScale = 1.0f;
        float textScale = deFaultTextScale;

        int depth = 0;

        bool wordGap = false;

        fvect3 textColor = fvect3(0.0f,0.0f,0.0f);
    };
}

