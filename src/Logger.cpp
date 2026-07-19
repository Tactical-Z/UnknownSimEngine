#include "Logger.h"
#include "ThreadContext.h"


void Logger::Set(Logger* _logger)
{
    sInstance = _logger;
}

const char* Logger::LevelToStr(LogLevel _level)
{
    switch (_level)
    {
        case LogLevel::_DEBUG_:
            return "[DEBUG] ";

        case LogLevel::_INFO_:
            return "[INFO] ";

        case LogLevel::_WARNING_:
            return "[WARNING] ";

        case LogLevel::_SUCCESS_:
            return "[SUCCESS] ";

        case LogLevel::_ERROR_:
            return "[ERROR] ";

        case LogLevel::_DEFAULT_:
            return "";
    }

    return "UNKNOWN";
}

std::string Logger::ThreadToStr(ThreadContext* _threadContext)
{
    if (_threadContext)
    {
        return std::format(
            "[{} {}] ",
            _threadContext->mID,
            _threadContext->mName
        );
    }

    return "[Main] ";
}

const char* Logger::LevelToColor(LogLevel _level)
{
 	switch (_level) {
 	case LogLevel::_DEBUG_:
 		return "\033[34m"; // Blue
 		break;
    case LogLevel::_SUCCESS_:
        return "\033[32m"; // Green
        break;
 	case LogLevel::_WARNING_:
 		return "\033[33m"; // Yellow
 		break;
 	case LogLevel::_ERROR_:
 		return "\033[31m"; // Red
 		break;
 	}
    return "\033[37m"; // white
}

void Logger::Write(LogLevel _level, const std::string& _message){

    std::cout << LevelToColor(_level) << LevelToStr(_level) << ThreadToStr(gCurrentThread) << _message << LevelToColor(LogLevel::_DEFAULT_) << std::endl;
}