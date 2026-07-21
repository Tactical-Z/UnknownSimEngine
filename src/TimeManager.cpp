#include "TimeManager.h"
#include "Log.h"

std::stack<std::chrono::high_resolution_clock::time_point> TimeManager::mTimerStack = {};
std::chrono::high_resolution_clock::time_point TimeManager::mAppStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point TimeManager::mLastFrameTime = std::chrono::high_resolution_clock::now();
float TimeManager::mDeltaTime = 0.0f;
float TimeManager::mFpsUpdateInterval = 0.5f;
float TimeManager::mFPS = 0.0f;
float TimeManager::mFPSTimer = 0.0f;

void TimeManager::Init()
{

}

void TimeManager::Update(){
    UpdateDeltaTime();
    UpdateFPS();
}

void TimeManager::StartTimer()
{
    mTimerStack.push(std::chrono::high_resolution_clock::now());
}

float TimeManager::EndTimer()
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

void TimeManager::UpdateDeltaTime(){
    
    auto currentTime = std::chrono::high_resolution_clock::now();

    mDeltaTime = std::chrono::duration<float>(currentTime - mLastFrameTime).count();

    mLastFrameTime = currentTime;
}

void TimeManager::UpdateFPS()
{
    mFPSTimer += mDeltaTime;
    if (mFPSTimer >= mFpsUpdateInterval)
    {
        mFPS = 1.0f / mDeltaTime;
        mFPSTimer = 0.0f;
    }
}

float TimeManager::GetDeltaTime()
{
    return mDeltaTime;
}

float TimeManager::GetFPS()
{
    return mFPS;
}

float TimeManager::GetTimeSinceAppStart()
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - mAppStartTime).count();
}