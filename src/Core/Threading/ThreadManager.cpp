#include "ThreadManager.h"

void ThreadManager::ThreadEntry(uint32_t _id, const char* _name, std::function<void()> _function)
{
    ThreadContext context;

    context.mID = _id;
    context.mName = _name;

    gCurrentThread = &context;

    _function();

    gCurrentThread = nullptr;
}