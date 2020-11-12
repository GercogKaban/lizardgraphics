#include "LTextEdit.h"

namespace LGraphics
{
    LTextEdit::LTextEdit(LApp * app, const std::string text_, const char* path, bool verticalScroller)
        :LText(app, text_, path, verticalScroller) 
    {
        wordGap = true;
    }
}