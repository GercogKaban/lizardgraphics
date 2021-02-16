#pragma once
#include "LLogger.h"
#include "pch.h"

namespace LGraphics
{
    LLogger::ThreadSafeQueue<std::string> LLogger::errors;
    uint8_t LLogger::states = 0;
    LApp* LLogger::app_;
    std::string LLogger::logFilePath = "log.txt";
    std::ios_base::_Openmode LSyncLogger::firstCreationFlag = std::ios_base::out;
    std::thread LAsyncLogger::thread;
}