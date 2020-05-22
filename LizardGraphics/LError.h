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
    /*!
    @brief Класс вывода ошибок.
    */
    class LError : public LObject
    {
    public:

        /*!
        @brief Инициализирует класс LError.
        */
        static void init()
        {
            errors.push_back("");
        }

        /*!
        @brief Выводит в консоль ошибку.
        */
        static void printToConsole(const std::string& error)
        {
            lastErrorNum++;
            std::cout << error.data() << std::endl;
        }

        /*!
        @brief Выводит на экран еще не выведенные ошибки.
        @warning Функция пока не рабочая, будет сделана после реализации вывода текста на экран.
        @todo Добавить определение.
        */
        static void printToDisplay(const std::string& error)
        {
            // ...
        }

        /*!
        @brief Записывает ошибку в массив ошибок.
        @param t - строка (const char* или std::string)
        */
        template <typename T>
        static void writeError(T t)
        {
            errors.back() += t;
            errors.push_back("");
        }

        /*!
        @brief Записывает ошибку в массив ошибок.
        @param t - строка (const char* или std::string)
        @param args - переменное кол-во строк (const char* или std::string)
        */
        template<typename T, typename... Args>
        static void writeError(T t, Args... args)
        {
            errors.back() += t;
            writeError(args...);
        }

        /*!
        @brief Выводит в консоль все не выведенные ошибки.
        */
        static void printErrors()
        {
            std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToConsole);
        }

        /*!
        @brief Освобождает ресурсы.
        */
        static void releaseResources()
        {
            errors.clear();
            lastErrorNum = 0;
        }

    protected:

        static std::vector<std::string> errors; ///< Коллекция ошибок.
        static size_t lastErrorNum;             ///< Индекс последней выведенной ошибки.
    };
}