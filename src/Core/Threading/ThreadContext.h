#pragma once

#include <cstdint>

class ThreadContext
{
public:
    uint32_t mID;
    const char* mName;
    
    static int mThreadIndexIterator;
    static void Set(uint32_t _id, const char* _name);
    static const ThreadContext& Get();
    static ThreadContext Copy();
    
private:
    static thread_local ThreadContext sContext;

};