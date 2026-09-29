#include "core/errorHandling/LogSink.h"
#include "core/errorHandling/Logger.h"

#include <iostream>

void ConsoleSink::Write(const LogEntry& _entry){
    
    if(_entry.mLogLevel <= LogLevel::LL_WARNING){
        std::cout
            << Logger::LevelToColor(_entry.mLogLevel)
            << Logger::LevelToChar(_entry.mLogLevel)
            << Logger::ThreadToChar(&_entry.mThreadContext)
            << _entry.mMessage
            << "\033[0m"
            << "\n";
    }
    else {
        std::cout
            << Logger::LevelToColor(_entry.mLogLevel)
            << Logger::LevelToChar(_entry.mLogLevel)
            << Logger::ThreadToChar(&_entry.mThreadContext)
            << _entry.mMessage << "\n"
            << "File: " << _entry.mSourceLocation.file_name() << "\n"
            << "Function: " << _entry.mSourceLocation.function_name() << "\n"
            << "Line/Col: " << _entry.mSourceLocation.line() << ":" << _entry.mSourceLocation.column() << "\n"
            << "\033[0m" 
            << "\n";
    }
}

void ConsoleSink::Clear()
{}

void ImGuiSink::Write(const LogEntry& _entry){
    mEntries.push_back(_entry);
}

void ImGuiSink::Clear(){
    mEntries.clear();
}

const std::vector<LogEntry>& ImGuiSink::GetEntries() const{
    return mEntries;
}