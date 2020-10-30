#pragma once
#include "LText.h"

namespace LGraphics
{  
    class LTextEdit : public LText
    {
    public:

        const char* getObjectType() override { return "LTextEdit"; }

        // необходимо поменять  местами 2-й и 3-й параметры
        LTextEdit(LApp* app, const std::string = "", const char* path = nullptr);
    };
}

