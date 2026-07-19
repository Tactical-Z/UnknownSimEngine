#pragma once

#include "Logger.h"

#define LOG_DEBUG(...) \
    Logger::Get().Log(LogLevel::_DEBUG_, __VA_ARGS__)

#define LOG_INFO(...) \
    Logger::Get().Log(LogLevel::_INFO_, __VA_ARGS__)

#define LOG_SUCCESS(...) \
    Engine::Get().Log(LogLevel::_SUCCESS_, __VA_ARGS__)

#define LOG_WARNING(...) \
    Logger::Get().Log(LogLevel::_WARNING_, __VA_ARGS__)

#define LOG_ERROR(...) \
    Logger::Get().Log(LogLevel::_ERROR_, __VA_ARGS__)