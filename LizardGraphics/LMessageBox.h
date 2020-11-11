#pragma once

#include "LText.h"

namespace LGraphics
{
    class LIButton;
    class LMessageBox : public LText
    {
    public:

        LMessageBox(LApp* app, const std::string text, const std::string buttonText = "OK", const char* path = nullptr);

        void move(const fvect3 val) override;
        void move(const float x, const float y, const float z) override;
            
        LIButton* getButton() { return button; }

    protected:

        LIButton* button;
    };
}

