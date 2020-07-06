#pragma once

#include <functional>
#include <chrono>
#include <future>

#include "vectors.h"
class LTimer
{
public:

    LTimer(std::function<void()> func, std::chrono::milliseconds interval = std::chrono::milliseconds(1000), bool start = false);

    void start();
    void stop();
    void restart();

    void setInterval(std::chrono::milliseconds interval) { this->interval = interval; }

protected:

    std::function<void()> func;
    std::chrono::milliseconds interval;
    bool isStopped = false;
};

