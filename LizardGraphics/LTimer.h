#pragma once

#include <functional>
#include <chrono>
#include <thread>

//#include "vectors.h"

class LTimer
{
public:

    LTimer(std::function<void()> func, size_t intervalMilliSec = 1000, bool start = false);
    ~LTimer();

    void start();
    void stop();
    void restart();

    void setInterval(std::chrono::milliseconds interval_) { this->interval = interval_; }
    
protected:

    
    std::thread thread;
    std::function<void()> func;
    std::chrono::milliseconds interval;
    bool isStopped = true;
};

