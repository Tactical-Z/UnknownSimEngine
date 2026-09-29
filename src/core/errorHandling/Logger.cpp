#include "core/errorHandling/Logger.h"

Logger::~Logger(){
    for(ISink* sink : mSinks){
        sink->Clear();
        delete sink;
        sink = nullptr;
    }
}


void Logger::AddSink(ISink* _sink){
    mSinks.push_back(_sink);
}

const char* Logger::LevelToColor(LogLevel _level)
{
 	switch (_level) {
    case LogLevel::LL_INFO:
 		return "\033[37m"; // white

    case LogLevel::LL_SUCCESS:
        return "\033[32m"; // Green

    case LogLevel::LL_DEBUG:
 		return "\033[34m"; // Blue

    case LogLevel::LL_DEBUG_ASSERT:
 		return "\033[34m"; // Blue

 	case LogLevel::LL_WARNING:
 		return "\033[33m"; // Yellow

 	case LogLevel::LL_ERROR:
 		return "\033[38;2;255;165;0m"; // Orange

    case LogLevel::LL_FATAL:
 		return "\033[31m"; // Red

    default:
 		return "\033[37m"; // white
 
 	}
    return "\033[37m"; // white
}

const char* Logger::LevelToChar(LogLevel _level)
{
    switch (_level)
    {
        case LogLevel::LL_INFO:
            return "[INFO] ";

        case LogLevel::LL_SUCCESS:
            return "[SUCCESS] ";

        case LogLevel::LL_DEBUG:
            return "[DEBUG] ";

        case LogLevel::LL_DEBUG_ASSERT:
 		    return "[DEBUG-ASSERT] ";

        case LogLevel::LL_WARNING:
            return "[WARNING] ";

        case LogLevel::LL_ERROR:
            return "[ERROR] ";

        case LogLevel::LL_FATAL:
            return "[FATAL] ";

        default:
            return "";
    }

    return "Err";
}

std::string Logger::ThreadToChar(const ThreadContext* _threadContext)
{
    if (_threadContext)
    {
        return std::format(
            "[{} {}] ",
            _threadContext->mID,
            _threadContext->mName
        );
    }

    return "[Unkown] ";
}
