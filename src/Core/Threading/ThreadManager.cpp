#include "ThreadManager.h"

void ThreadManager::ThreadEntry(uint32_t _id, const char* _name, std::function<void(std::stop_token)> _function)
{
    mThread = std::jthread(
        [_id, _name, _function = std::move(_function)](std::stop_token _stop)
        {
            ThreadContext::Set(_id, _name);
            _function(_stop);
        });
}
