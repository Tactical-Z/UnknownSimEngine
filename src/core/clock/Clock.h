#pragma once 

#include <chrono>
#include <stack>
#define NOMINMAX
#include <Windows.h>

class Clock{
public:

    explicit Clock() = default;
    explicit Clock(class Logger* _logger);
    ~Clock() = default;

    void Update();
    void StartTimer();
    float EndTimer();

    float GetDeltaTime();
    float GetTimeSinceAppStart();
    float GetAvgFPS();
    float GetAvgFrameTimeMs();
    float GetAvgCPUUsage();

private:

    class Logger* mLogger;
    std::stack<std::chrono::high_resolution_clock::time_point> mTimerStack;
    std::chrono::high_resolution_clock::time_point mAppStartTime;
    std::chrono::high_resolution_clock::time_point mLastFrameTime;
    bool mFirstFrame = true;
    
    float mDeltaTime = 0.0f;
    void UpdateDeltaTime();

    static constexpr float mPerformanceUpdateInterval = 0.25f; // how often to update clock data
    float mPerformanceTimer = 0.f;
    void UpdatePerformanceData();

    float mFPSTimer = 0.0f;
    int mFrameCount = 0;
    float mAvgFPS = 0.f;
    
    float mFrametimeTimer = 0.0f;
    float mFrameTimeAccumulated = 0.0f;
    float mAvgFrameTimeMs = 0.0f;

    ULONGLONG mLastProcessKernelTime = 0;
    ULONGLONG mLastProcessUserTime = 0;
    ULONGLONG mLastSystemKernelTime = 0;
    ULONGLONG mLastSystemUserTime = 0;
    float mCPUUsageAccumulated = 0.0f;
    float mAvgCPUUsage = 0.0f;
    float GetCurrentCPUUsage();

};