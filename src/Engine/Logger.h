#pragma once

#include <string>
#include <mutex>
#include <format>
#include <iostream>
#include <cassert>

enum class LogLevel
{
	_DEFAULT_,
	_DEBUG_,
	_INFO_,
    _SUCCESS_,
	_WARNING_,
	_ERROR_
};

class Logger
{
public:
    Logger() = default;
    ~Logger() = default;

    static void Set(Logger* logger);

    static Logger& Get()
    {
        assert(sInstance && "Logger not initialized!");
        return *sInstance;
    }

    template<typename... Args>
    void Log(LogLevel _level, std::format_string<Args...> _fmt, Args&&... _args)
    {
        
        std::string message = std::format (_fmt, std::forward<Args>(_args)...);

        std::lock_guard<std::mutex> lock(mMutex);
        Write(_level, message);
    
    }

private:
    
    inline static Logger* sInstance = nullptr;
    std::mutex mMutex; // locks and unlocks threads so multiple messages cant be sent at the same time, for a larger system replace with a queue system.

    const char* LevelToStr(LogLevel _level);
    std::string ThreadToStr(struct ThreadContext* _threadContext);
    const char* LevelToColor(LogLevel _level);
    void Write(LogLevel _level, const std::string& _message);    

};