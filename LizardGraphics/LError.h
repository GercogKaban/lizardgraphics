#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include "LObject.h"

#define LWRITE(...) LError::writeError(__VA_ARGS__)
#define LPRINT(...) LError::printErrors()
#define LDISPLAY() LError::displayErrors()

namespace LGraphics
{
    class LError : public LObject
    {
    public:

        static void init()
        {
            errors.push_back("");
        }

        static void printToConsole(const std::string& error)
        {
            lastErrorNum++;
            std::cout << error.data() << std::endl;
        }

        static void printToDisplay(const std::string& error)
        {
            // ...
        }

        template <typename T>
        static void writeError(T t)
        {
            errors.back() += t;
            errors.push_back("");
        }

        template<typename T, typename... Args>
        static void writeError(T t, Args... args)
        {
            errors.back() += t;
            writeError(args...);
        }

        static void printErrors()
        {
            std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToConsole);
        }

        static void releaseResources()
        {
            errors.clear();
            lastErrorNum = 0;
        }

    protected:

        static std::vector<std::string> errors;
        static size_t lastErrorNum;
    };
}