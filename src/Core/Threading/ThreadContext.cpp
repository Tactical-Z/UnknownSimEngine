#include "core/threading/ThreadContext.h"

thread_local ThreadContext ThreadContext::sContext{0, "Unknown"};
int ThreadContext::mThreadIndexIterator = 0;

void ThreadContext::Set(uint32_t _id, const char* _name)
{
    sContext.mID = _id;
    sContext.mName = _name;
}

const ThreadContext& ThreadContext::Get()
{
    return sContext;
}

ThreadContext ThreadContext::Copy()
{
    return sContext;
}