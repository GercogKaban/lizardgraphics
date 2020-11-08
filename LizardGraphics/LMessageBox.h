#pragma once

#include "LText.h"

namespace LGraphics
{
    class LMessageBox : LText
    {
    public:

        LMessageBox(LApp* app, const std::string text, const std::string buttonText = "OK", const char* path = nullptr);
    };
}

