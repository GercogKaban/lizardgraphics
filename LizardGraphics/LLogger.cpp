#pragma once
#include "LLogger.h"
#include "pch.h"

namespace LGraphics
{
    LLogger::ThreadSafeQueue<std::string> LLogger::errors;
    std::stack<std::string> LLogger::calls;
    uint8_t LLogger::states = 0;
    LApp* LLogger::app_;
    std::string LLogger::logFilePath = "log.txt";
    std::ios_base::openmode LSyncLogger::firstCreationFlag = std::ios_base::out;
    std::thread LAsyncLogger::thread;
    bool LLogger::stopLogging = false;
    bool LAsyncLogger::started;

    LogVar::LogVar(const std::string& str)
    {
        LLogger::calls.push(str);
    }

    LogVar::~LogVar()
    {
        LLogger::calls.pop();
    }

}
