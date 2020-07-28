#include "LTimer.h"
#include <thread>
#include <iostream>
#include "LLine.h"

LTimer::LTimer(std::function<void()> func, std::chrono::milliseconds interval,bool start_)
{
    this->func = func;
    this->interval = interval;
    if (start_) start();
}

void LTimer::start()
{
    isStopped = false;
    std::thread thread(std::move([&]() { while (!isStopped) {std::this_thread::sleep_for(interval); func(); } }));
    thread.detach();
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
