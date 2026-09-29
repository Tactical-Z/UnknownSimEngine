#pragma once

#include "core/errorHandling/LogLevel.h"
#include "core/threading/ThreadContext.h"

#include <string>
#include <chrono>
#include <thread>
#include <source_location>

struct LogEntry{

    LogLevel mLogLevel;
    std::string mMessage;
    std::chrono::system_clock::time_point mTimeStamp;
    ThreadContext mThreadContext;
    std::source_location mSourceLocation;
};