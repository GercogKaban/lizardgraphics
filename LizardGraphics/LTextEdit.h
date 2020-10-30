#pragma once
#include "LText.h"

namespace LGraphics
{  
    class LTextEdit : public LText
    {
    public:

        const char* getObjectType() override { return "LTextEdit"; }

        // ���������� ��������  ������� 2-� � 3-� ���������
        LTextEdit(LApp* app, const std::string = "", const char* path = nullptr);
    };
}

