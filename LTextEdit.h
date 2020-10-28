#pragma once
#include "LText.h"

namespace LGraphics
{
    class LTextEdit : public LText
    {
    public:
        LTextEdit(LApp* app, const std::string = "", const char* path = nullptr);
    };
}

