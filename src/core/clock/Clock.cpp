#include "core/clock/Clock.h"
#include "core/errorHandling/Log.h"

Clock::Clock(Logger* _logger)
    :   mLogger(_logger),
        mAppStartTime(std::chrono::high_resolution_clock::now()),
        mLastFrameTime(mAppStartTime)
{}

void Clock::Update(){
    UpdateDeltaTime();
    UpdatePerformanceData();
}

void Clock::StartTimer()
{
    mTimerStack.push(std::chrono::high_resolution_clock::now());
}

float Clock::EndTimer()
{
    if(mTimerStack.empty()){
        LOG_WARNING("No timers pending");
        return 0;
    }
    
    std::chrono::high_resolution_clock::time_point starTime = mTimerStack.top();
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();

    float duration = std::chrono::duration<float>(endTime - starTime).count();
    mTimerStack.pop();

    return duration;
}

void Clock::UpdateDeltaTime(){
    
    auto currentTime = std::chrono::high_resolution_clock::now();

    if (mFirstFrame)
    {
        mLastFrameTime = currentTime;
        mDeltaTime = 0.0f;
        mFirstFrame = false;
        return;
    }

    mDeltaTime = std::chrono::duration<float>(currentTime - mLastFrameTime).count();
    mLastFrameTime = currentTime;
}

float Clock::GetCurrentCPUUsage()
{
    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;

    FILETIME idleTime;
    FILETIME systemKernelTime;
    FILETIME systemUserTime;

    if (!GetProcessTimes(
        GetCurrentProcess(),
        &creationTime,
        &exitTime,
        &kernelTime,
        &userTime))
    {
        return 0.0f;
    }

    if (!GetSystemTimes(
        &idleTime,
        &systemKernelTime,
        &systemUserTime))
    {
        return 0.0f;
    }

    ULARGE_INTEGER processKernel;
    ULARGE_INTEGER processUser;
    ULARGE_INTEGER systemKernel;
    ULARGE_INTEGER systemUser;

    processKernel.LowPart  = kernelTime.dwLowDateTime;
    processKernel.HighPart = kernelTime.dwHighDateTime;

    processUser.LowPart  = userTime.dwLowDateTime;
    processUser.HighPart = userTime.dwHighDateTime;

    systemKernel.LowPart  = systemKernelTime.dwLowDateTime;
    systemKernel.HighPart = systemKernelTime.dwHighDateTime;

    systemUser.LowPart  = systemUserTime.dwLowDateTime;
    systemUser.HighPart = systemUserTime.dwHighDateTime;

    ULONGLONG processTime =
        (processKernel.QuadPart - mLastProcessKernelTime) +
        (processUser.QuadPart - mLastProcessUserTime);

    ULONGLONG systemTime =
        (systemKernel.QuadPart - mLastSystemKernelTime) +
        (systemUser.QuadPart - mLastSystemUserTime);

    mLastProcessKernelTime = processKernel.QuadPart;
    mLastProcessUserTime   = processUser.QuadPart;
    mLastSystemKernelTime  = systemKernel.QuadPart;
    mLastSystemUserTime    = systemUser.QuadPart;

    if (systemTime == 0)
        return 0.0f;

    return static_cast<float>(processTime) /
           static_cast<float>(systemTime) *
           100.0f;
}

void Clock::UpdatePerformanceData(){

    if (mFirstFrame)
        return;

    mPerformanceTimer += mDeltaTime;

    mFrameTimeAccumulated += mDeltaTime;
    mCPUUsageAccumulated += GetCurrentCPUUsage();
    ++mFrameCount;

    if (mPerformanceTimer >= mPerformanceUpdateInterval)
    {
        // Average frame time
        if (mFrameCount > 0)
        {
            mAvgFrameTimeMs =
                (mFrameTimeAccumulated / mFrameCount)
                * 1'000.0f;

            // Average FPS
            mAvgFPS =
                static_cast<float>(mFrameCount) /
                mPerformanceTimer;

            // Average CPU usage
            mAvgCPUUsage =
                mCPUUsageAccumulated / mFrameCount;
        }

        mPerformanceTimer = 0.0f;
        mFrameTimeAccumulated = 0.0f;
        mCPUUsageAccumulated = 0.0f;
        mFrameCount = 0;
    }
}

float Clock::GetDeltaTime()
{
    return mDeltaTime;
}

float Clock::GetTimeSinceAppStart()
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - mAppStartTime).count();
}

float Clock::GetAvgFPS()
{
    return mAvgFPS;
}

float Clock::GetAvgFrameTimeMs()
{
    return mAvgFrameTimeMs;
}

float Clock::GetAvgCPUUsage()
{
    return mAvgCPUUsage;
}