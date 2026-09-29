#pragma once
#include "core/threading/ThreadContext.h"

#include <thread>
#include <functional>


class ThreadManager
{
public:
    ThreadManager() = default;
    ~ThreadManager() = default;

    void ThreadEntry(uint32_t _id, const char* _name, std::function<void(std::stop_token)> function);

private:
    std::jthread mThread;
};