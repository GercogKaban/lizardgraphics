#pragma once
#include "LTextEdit.h"
#include <sstream>

namespace LGraphics
{
    class LCounter : public LTextEdit
    {
    public:

        const char* getObjectType() override { return "LCounter"; }

        void move(const fvect3 val) override;
        void move(const float x, const float y, const float z) override;

        void scale(const fvect3 val) override;
        void scale(const float x, const float y, const float z) override;

        void addText(const unsigned int symbol) {}
        void addText(const std::string text) override;

        virtual void addNumber(const double val) = 0;

        virtual void setCountInterval(double interval) = 0; //{ countInterval = interval; }
        void setOutPrecision(const size_t precision) { outPrecision = precision; }
        void setChangingEventFunc(std::function<void()> f) { changingEventFunc = f; }
        //void clear() override;

        virtual double getNum() const = 0;

    protected:

        LCounter(LApp* app, const std::string = "", const char* path = nullptr);

        template <typename T>
        std::string to_string_with_precision(const T a_value, const int n = 6)
        {
            std::ostringstream out;
            out.precision(n);
            out << std::fixed << a_value;
            return out.str();
        }

        size_t outPrecision = 6;
        std::function<void()> changingEventFunc;
        void calculateMaxLength() override;
    };

    class LCounterI : public LCounter
    {
    public:

        const char* getObjectType() override { return "LCounterI"; }
        LCounterI(LApp* app, const std::string = "", const char* path = nullptr);

        void addNumber(const double val) override;

        void setCountInterval(double interval) override;
        void addText(const unsigned int symbol) override;
        void removeLastSymbol() override;
        void clear() override;

        double getNum() const override;

    protected:

        int counter = 0;
        int countInterval = 1;
    };

    class LCounterF : public LCounter
    {
    public:

        const char* getObjectType() override { return "LCounterF"; }
        LCounterF(LApp* app, const std::string = "", const char* path = nullptr);

        void addNumber(const double val) override;
        void setCountInterval(double interval) override;
        void addText(const unsigned int symbol) override;
        void removeLastSymbol() override;
        void clear() override;

        double getNum() const override;

    protected:

        float counter = 0;
        float countInterval = 1.0f;
    };

    class LCounterLL : public LCounter
    {
    public:

        const char* getObjectType() override { return "LCounterLL"; }
        LCounterLL(LApp* app, const std::string = "", const char* path = nullptr);

        void addNumber(const double val) override;
        void setCountInterval(double interval) override;
        void addText(const unsigned int symbol) override;
        void removeLastSymbol() override;
        void clear() override;

        double getNum() const override;

    protected:

        long long counter = 0;
        long long countInterval = 1.0f;
    };
}