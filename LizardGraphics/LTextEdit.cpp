#include "LTextEdit.h"

namespace LGraphics
{
    LTextEdit::LTextEdit(LApp * app, const std::string text_, LObject* parent, const char* path, LBaseComponent* component)
        :LText(app, text_, parent, path, component) {}
}