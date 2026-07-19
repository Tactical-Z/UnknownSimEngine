#pragma once

#include <cstdint>

struct ThreadContext{

    uint32_t mID;
    const char* mName;
};

extern thread_local ThreadContext* gCurrentThread;