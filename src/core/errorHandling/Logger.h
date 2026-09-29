#pragma once
#include "core/errorHandling/LogSink.h"
#include "core/threading/ThreadContext.h"

#include <format>
#include <mutex>

class Logger
{
public:
    explicit Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;

    void AddSink(ISink* _sink);

    template <typename... Args>
    void Log(LogLevel _level, std::source_location _sourceLocation, std::format_string<Args...> _format, Args&&... _args){
    
        LogEntry entry{
            _level,
            std::format(_format, std::forward<Args>(_args)...),
            std::chrono::system_clock::now(), // TODO: time context goes here, how long since thread (main) start, not needed yet and dependent on threading so left at x
            ThreadContext::Copy(), // TODO: thead context goes here, not fully implemented yet so left at null 
            _sourceLocation
        };

        std::lock_guard lock(mMutex);

        for (ISink* sink : mSinks)
        {
            sink->Write(entry);
        }
    }

    template <typename T>
    void Clear(){
        if (T* sink = GetSink<T>())
            sink->Clear();
    }

    template <typename T>
    T* GetSink(){
        for(ISink* sink : mSinks)
            if(T* sinkEntry = dynamic_cast<T*>(sink))
                return sinkEntry;
        return nullptr;
    }

    static const char* LevelToColor(LogLevel _level);
    static const char* LevelToChar(LogLevel _level);
    static std::string ThreadToChar(const struct ThreadContext* _threadContext);

private:
    std::vector<ISink*> mSinks;
    std::mutex mMutex;
};