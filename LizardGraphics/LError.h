#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

#include "LObject.h"

//#define LWRITE(...) LError::writeError(__VA_ARGS__)
#define PRINT(...) LError::printMsg(__VA_ARGS__)
#define PRINTLN(...) LError::printMsgLn(__VA_ARGS__)
//#define LDISPLAY() LError::displayErrors()

namespace LGraphics
{
    class LApp;
    /*!
    @brief Класс вывода ошибок.
    */
    class LError : public LObject
    {
    public:

        const char* getObjectType() const override { return "LError"; }
        /*!
        @brief Инициализирует класс LError.
        */
        static void initErrors(LApp* app)
        {
            app_ = app;
            errors.push_back("");
        }

        /*!
        @brief Освобождает ресурсы.
        */
        static void releaseResources()
        {
            errors.clear();
            lastErrorNum = 0;
        }

        template<typename T>
        static void print(T text)
        {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            LOGI(tag_, text);
#else
            std::cout << text;
#endif
        }

        template<typename T>
        static void println(T text)
        {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            LOGI(tag_, text);
#else
            std::cout << text << std::endl;
#endif
        }


        /*!
        @brief Выводит в консоль ошибку.
        */
        static void printToConsole(const std::string& error)
        {
            lastErrorNum++;
            print(error.data());
        }

        /*!
        @brief Выводит на экран еще не выведенные ошибки.
        @warning Функция пока не рабочая, будет сделана после реализации вывода текста на экран.
        @todo Добавить определение.
        */
        static void printToDisplay(const std::string& error)
        {
            lastErrorNum++;
            //auto msg = new LGraphics::LMessageBox(app_,error);
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
        static void printMsg(T t, Args... args)
        {
            printMsg(t);
            printMsg(args...);
        }

        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
*/
        template <typename T>
        static void printMsg(T t)
        {
            print(t);
        }


        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
@param args - переменное кол-во строк (const char* или std::string)
*/
        template<typename T, typename... Args>
        static void printMsgLn(T t, Args... args)
        {
            printMsg(t);
            printMsgLn(args...);
        }

        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
*/
        template <typename T>
        static void printMsgLn(T t)
        {
            println(t);
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

        static void displayErrors()
        {
            std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToDisplay);
        }

protected:
        static LApp* app_;
        static std::vector<std::string> errors; ///< Коллекция ошибок.
        static size_t lastErrorNum;             ///< Индекс последней выведенной ошибки.
    };
}