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

    void setInterval(std::chrono::milliseconds interval_) { this->interval = interval_; }

protected:

    std::function<void()> func;
    std::chrono::milliseconds interval;
    bool isStopped = false;
};

