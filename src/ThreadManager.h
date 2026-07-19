#pragma once

#include <thread>
#include <functional>
#include "ThreadContext.h"

class ThreadManager
{
public:
    ThreadManager() = default;
    ~ThreadManager() = default;

    void ThreadEntry(uint32_t _id, const char* _name, std::function<void()> _function);

private:



};