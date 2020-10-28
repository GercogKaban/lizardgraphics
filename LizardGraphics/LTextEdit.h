#pragma once
#include "LText.h"

namespace LGraphics
{
    class LTextEdit : public LText
    {
    public:

        const char* getObjectType() override { return "LTextEdit"; }
        LTextEdit(LApp* app, const std::string = "", const char* path = nullptr);
    };
}

