#pragma once 

#include <chrono>
#include <stack>

class STime{
public:

    STime() = default;
    ~STime() = default;

    static void Init();
    static void Update();
    static void StartTimer();
    static float EndTimer();

private:
    static std::stack<std::chrono::high_resolution_clock::time_point> mTimerStack;
    static std::chrono::high_resolution_clock::time_point mAppStartTime;
    static std::chrono::high_resolution_clock::time_point mLastFrameTime;
    static float mDeltaTime;
    static float mFpsUpdateInterval; // how often to update FPS in seconds.
    static float mFPS;
    static float mFPSTimer;

    static void UpdateFPS();
    static void UpdateDeltaTime();

public:
    static float GetDeltaTime();
    static float GetFPS();
    static float GetTimeSinceAppStart();

};