#pragma once

#include "core/errorHandling/Logger.h"
#include <cassert>

// Strictly terrable programming practice, never assume the precence of a verraiable (mLogger), but i am lazy.
#ifndef NDEBUG

    #define LOG_DEBUG(...) \
        if(mLogger) { mLogger->Log(LogLevel::LL_DEBUG, std::source_location::current(), __VA_ARGS__);}

    #define LOG_ASSERT(condition, message, ...) \
          if(mLogger) { mLogger->Log(LogLevel::LL_DEBUG_ASSERT, std::source_location::current(), "Condition: '" #condition "' Message: " message __VA_OPT__(, ) __VA_ARGS__);}; \
        assert(condition)
#else

    #define LOG_DEBUG(...) \
        ((void)0)

    #define LOG_ASSERT(condition, message, ...) \
        ((void)0)

#endif

    #define LOG_INFO(...) \
          if(mLogger) { mLogger->Log(LogLevel::LL_INFO, std::source_location::current(), __VA_ARGS__);}

    #define LOG_SUCCESS(...) \
          if(mLogger) { mLogger->Log(LogLevel::LL_SUCCESS, std::source_location::current(), __VA_ARGS__);}

    #define LOG_WARNING(...) \
          if(mLogger) { mLogger->Log(LogLevel::LL_WARNING, std::source_location::current(), __VA_ARGS__);}

    #define LOG_ERROR(...) \
         if(mLogger) { mLogger->Log(LogLevel::LL_ERROR, std::source_location::current(), __VA_ARGS__);}

    #define LOG_FATAL(...) \
          if(mLogger) { mLogger->Log(LogLevel::LL_FATAL, std::source_location::current(), __VA_ARGS__);}; \
        std::abort()
    
