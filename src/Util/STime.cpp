#include "Util/STime.h"
#include "Util/Log.h"

std::stack<std::chrono::high_resolution_clock::time_point> STime::mTimerStack = {};
std::chrono::high_resolution_clock::time_point STime::mAppStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point STime::mLastFrameTime = std::chrono::high_resolution_clock::now();
float STime::mDeltaTime = 0.0f;
float STime::mFpsUpdateInterval = 0.5f;
float STime::mFPS = 0.0f;
float STime::mFPSTimer = 0.0f;

void STime::Init()
{

}

void STime::Update(){
    UpdateDeltaTime();
    UpdateFPS();
}

void STime::StartTimer()
{
    mTimerStack.push(std::chrono::high_resolution_clock::now());
}

float STime::EndTimer()
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

void STime::UpdateDeltaTime(){
    
    auto currentTime = std::chrono::high_resolution_clock::now();

    mDeltaTime = std::chrono::duration<float>(currentTime - mLastFrameTime).count();

    mLastFrameTime = currentTime;
}

void STime::UpdateFPS()
{
    mFPSTimer += mDeltaTime;
    if (mFPSTimer >= mFpsUpdateInterval)
    {
        mFPS = 1.0f / mDeltaTime;
        mFPSTimer = 0.0f;
    }
}

float STime::GetDeltaTime()
{
    return mDeltaTime;
}

float STime::GetFPS()
{
    return mFPS;
}

float STime::GetTimeSinceAppStart()
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - mAppStartTime).count();
}