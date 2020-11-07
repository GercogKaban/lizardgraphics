#pragma once
#include "LTextEdit.h"
#include <sstream>

namespace LGraphics
{
    class LCounter : public LTextEdit
    {
    public:

        enum LCounterModes 
        {
            Integer,
            Float,
        };

        const char* getObjectType() override { return "LCounter"; }
        LCounter(LApp* app, const std::string = "", const char* path = nullptr);

        void move(const fvect3 val) override;
        void move(const float x, const float y, const float z) override;

        void scale(const fvect3 val) override;
        void scale(const float x, const float y, const float z) override;

        void addText(const unsigned int symbol) override;
        void addText(const std::string text) override;
        void removeLastSymbol() override;

        void setCountInterval(float interval) { countInterval = interval; }
        void setMode(int mode);
        void setFloatPrecision(const size_t precision) { floatPrecision = precision; }
        bool getMode() const { return mode; }

        void clear() override;

        float getNum() const { return mode == Integer? counterInt : counterFl; }

        template <typename T>
        std::string to_string_with_precision(const T a_value, const int n = 6)
        {
            std::ostringstream out;
            out.precision(n);
            out << std::fixed << a_value;
            return out.str();
        }

    protected:

        int mode = Integer;

        int counterInt = 0;
        float counterFl = 0.0f;
        float countInterval = 1.0f;

        size_t floatPrecision = 6;
    };
}