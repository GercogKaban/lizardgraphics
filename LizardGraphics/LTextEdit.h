#pragma once
#include "LIButton.h"
#include "LText.h"

namespace LGraphics
{
    class LTextEdit : public LIButton
    {
    public:

        LTextEdit(LApp* app, const std::string = "", LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);
        void draw() override;
        void scale(const fvect3 val) override;

        std::vector<Text> getText() const;

        void setText(const std::string& text);
        void addText(const std::string text);
        void addText(const unsigned int symbol);
        void removeLastSymbol();

    protected:

        std::vector<Text>::const_iterator begin;
        std::vector<Text>::const_iterator end;

        void initWidget();

        void calculateMaxLength();

        void yAlign();
        void alignText();

        bool outOfBordersY(float y);

        void pushNewString();

        float leftBorder = 0.038f;
        float rightBorder = 0.03f;
        float topBorder = 0.085f;
        float bottomBorder = 0;

        float strIndent = 0.1f;
        std::vector<Text> text;

        size_t hiddenStrings = 0;
        size_t maxStrings = 0;
        float maxLength = 0.0f;
    };
}

