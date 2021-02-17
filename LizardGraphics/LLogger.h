#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <mutex>
#include <fstream>
#include <chrono>
#include <ctime>

#include "LApp.h"

//#define LOG_HEADER std::string(std::ctime(std::move(&std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())))) +\
//" file: " + __FILE__ + " line: " + __LINE__


namespace LGraphics
{
    struct LogVar
    {
        LogVar(const std::string& str);
        ~LogVar();
    };

#define LOG_HEADER LGraphics::LLogger::getCurrentTime() + std::string("file: ") + __FILE__ + " line: " + std::to_string(__LINE__) + '\n'
#define PRINT(...) LGraphics::LLogger::printMsg(__VA_ARGS__)
#define PRINTLN(...) LGraphics::LLogger::printMsgLn(__VA_ARGS__)
#define LOG_CALL LGraphics::LogVar llogvar__(std::string(LOG_HEADER) + __FUNCTION__ + "\n");

    enum LLogStates : uint8_t
    {
        SYNC_LOG =            0b00000001, 
        ASYNC_LOG =           0b00000010,
        CONSOLE_DEBUG_LOG =   0b00000100,
        CONSOLE_RELEASE_LOG = 0b00001000,
        FILE_DEBUG_LOG =      0b00010000,
        FILE_RELEASE_LOG =    0b00100000,
    };

    /*!
    @brief Класс вывода ошибок.
    */
    class LLogger : public LObject
    {
    public:

        friend LogVar;
        friend LApp;

        template <typename T>
        class ThreadSafeQueue : private std::queue<T>
        {
        public:

            _NODISCARD bool empty_() const noexcept(noexcept(this->empty()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->empty();
            }

            _NODISCARD size_t size_() noexcept(noexcept(this->size()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->size();
            }

            _NODISCARD T& front_() noexcept(noexcept(this->front()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->front();
            }

            _NODISCARD const T& front_() const noexcept(noexcept(this->front()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->front();
            }

            _NODISCARD T& back_() noexcept(noexcept(this->back()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->back();
            }

            _NODISCARD const T& back_() const noexcept(noexcept(this->back()))
            {
                const std::lock_guard<std::mutex> lock(mutex);
                return this->back();
            }

            void push_(const T& val)
            {
                const std::lock_guard<std::mutex> lock(mutex);
                this->push(val);
            }

            void push_(const T&& val)
            {
                const std::lock_guard<std::mutex> lock(mutex);
                this->push(val);
            }

            void pop_()
            {
                const std::lock_guard<std::mutex> lock(mutex);
                this->pop();
            }

            void swap_(ThreadSafeQueue<T>& _Right)
            {
                const std::lock_guard<std::mutex> lock(mutex);
                this->swap(_Right);
            }

        private:
            mutable std::mutex mutex;
        };

        const char* getObjectType() const override { return "LLogger"; }

        /*!
        @brief Инициализирует класс LLogger.
        */
        static void initErrors(LApp* app)
        {
            app_ = app;
            states = app->getLogFlags();
            //errors.push_back("");
        }

        /*!
        @brief Освобождает ресурсы.
        */
        static void releaseResources()
        {
            //errors.clear();
            //lastErrorNum = 0;
        }

        template<typename T>
        static void print(const T& text)
        {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            LOGI(tag_, text);
#else
            std::cout << text;
#endif
        }

        template<typename T>
        static void println(const T& text)
        {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            LOGI(tag_, text);
#else
            std::cout << text << std::endl;
#endif
        }

        /*!
        @brief Записывает ошибку в массив ошибок.
        @param t - строка (const char* или std::string)
        */
        template <typename T>
        static void writeError(const T& t)
        {
            errors.push_(t);
        }

        /*!
        @brief Записывает ошибку в массив ошибок.
        @param t - строка (const char* или std::string)
        @param args - переменное кол-во строк (const char* или std::string)
        */
        template<typename T, typename... Args>
        static void printMsg(const T& t, Args... args)
        {
            printMsg(t);
            printMsg(args...);
        }

        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
*/
        template <typename T>
        static void printMsg(const T& t)
        {
            errors.push_(t);
        }

        template <>
        static void printMsg(const char& t)
        {
            errors.push_(std::string(t,1));
        }

        template <>
        static void printMsg(const size_t& t)
        {
            errors.push_(std::to_string(t));
        }


        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
@param args - переменное кол-во строк (const char* или std::string)
*/
        template<typename T, typename... Args>
        static void printMsgLn(const T& t, Args... args)
        {
            printMsg(t);
            printMsgLn(args...);
        }

        /*!
@brief Записывает ошибку в массив ошибок.
@param t - строка (const char* или std::string)
*/
        template <typename T>
        static void printMsgLn(const T& t)
        {
            errors.push_(t);
            errors.push_("\n");
            //println(t);
        }

        template <>
        static void printMsgLn(const char& t)
        {
            errors.push_(std::string(t,1));
        }

        /*!
        @brief Записывает ошибку в массив ошибок.
        @param t - строка (const char* или std::string)
        @param args - переменное кол-во строк (const char* или std::string)
        */
        template<typename T, typename... Args>
        static void writeError(const T& t, Args... args)
        {
            errors.push_(t);
            writeError(args...);
        }

        /*!
        @brief Выводит в консоль все не выведенные ошибки.
        */
        //static void printErrors()
        //{
        //    std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToConsole);
        //}

        //static void displayErrors()
        //{
        //    std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToDisplay);
        //}

        static std::string getCurrentTime()
        {
            auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            return std::string(std::ctime(&time));
        }

protected:
           static bool stopLogging;
//public:
        //static void stopLogging()
        //{
        //    stopLogging = true;
        //}

public:

protected:
  
    static void logToFile(std::string msg,
        std::ios_base::_Openmode firstCreationFlag = std::ios_base::app)
    {
        std::ofstream out(logFilePath, std::ios_base::binary | firstCreationFlag);
        if (!out.is_open())
            throw std::runtime_error("wrong path");
        out.write(msg.data(), msg.size());
        out.close();
    }

        static LApp* app_;

        static ThreadSafeQueue<std::string> errors;
        static std::stack<std::string> calls;
        static std::string logFilePath;

        static uint8_t states;
    };

    //static void runtimeException(const std::string& str)
    //{
    //    PRINTLN(str);
    //    throw runtimeExceptionCode;
    //}

    class LAsyncLogger;

    class LSyncLogger : public LLogger
    {
    public:
        friend LAsyncLogger;
        static void tick()
        {
            while (true)
            {
                while (errors.size_())
                {
                    auto err = errors.front_();
                    errors.pop_();
#ifndef NDEBUG
                if (states & CONSOLE_DEBUG_LOG)
                    print(err);
                if (states & FILE_DEBUG_LOG)
                    logToFile(err, firstCreationFlag);
#else
                if (states & CONSOLE_RELEASE_LOG)
                    println(err);
                if (states & FILE_RELEASE_LOG)
                    logToFile(err);
#endif
                firstCreationFlag = std::ios_base::app;
                }

                if (stopLogging)
                    return;
            }
        }

        static void emergencyStop()
        {
            stop();
            printCalls();
        }

        static void stop()
        {
            stopLogging = true;
            tick();
        }

    private:

        static void printCalls()
        {
            while (calls.size())
            {
                auto err = calls.top();
                calls.pop();
#ifndef NDEBUG
                if (states & CONSOLE_DEBUG_LOG)
                    print(err);
                if (states & FILE_DEBUG_LOG)
                    logToFile(err, firstCreationFlag);
#else
                if (states & CONSOLE_RELEASE_LOG)
                    println(err);
                if (states & FILE_RELEASE_LOG)
                    logToFile(err);
#endif
                firstCreationFlag = std::ios_base::app;
            }
    }

        static std::ios_base::_Openmode firstCreationFlag;
    };

    class LAsyncLogger : private LSyncLogger
    {
    public:

        static void emergencyStop()
        {
            stop();
            printCalls();
        }

        static void stop()
        {
            stopLogging = true;
            tick();
            thread.join();
        }

        static void start()
        {
            started = true;
            thread = std::thread(&LSyncLogger::tick);
        }

        static bool isStarted()
        {
            return started;
        }
    protected:

        static std::thread thread;
        static bool started;
    };
}
