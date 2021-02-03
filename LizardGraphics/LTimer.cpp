#include "pch.h"
#include "LTimer.h"

LTimer::LTimer(std::function<void()> func, size_t interval,bool start_)
{
    this->func = func;
    this->interval = std::chrono::milliseconds(interval);
    if (start_) start();
}

LTimer::~LTimer()
{
    stop();
    thread.join();
}

void LTimer::start()
{
    isStopped = false;
    thread = std::thread([&]() { while (!isStopped) { std::this_thread::sleep_for(interval); func(); }});
}

void LTimer::stop()
{
    isStopped = true;
}

void LTimer::restart()
{
    stop();
    start();
}
