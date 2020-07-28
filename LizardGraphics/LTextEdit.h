#pragma once
#include "LText.h"

namespace LGraphics
{
    class LTextEdit : public LText
    {
    public:
        LTextEdit(LApp* app, const std::string = "", LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);
    };
}

