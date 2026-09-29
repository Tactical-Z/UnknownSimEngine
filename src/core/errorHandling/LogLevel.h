#pragma once

// value dictates loging paramaters (enum = 1 != enum = 5)
enum class LogLevel{
    LL_INFO = 0,
    LL_SUCCESS = 1,
    LL_DEBUG = 2,
    LL_WARNING = 3,
    LL_DEBUG_ASSERT = 4,
    LL_ERROR = 5,
    LL_FATAL = 6
};