#pragma once

#include "engine/EngineConfig.h"
#include "engine/WindowManager.h"
#include "engine/Rendering/Renderer.h"
#include "engine/Rendering/UIManager.h"
#include "engine/Simulation/SimulationManager.h"

//Todo: add error detection for glFunctions
class Engine
{
public:
    Engine() = default;
    Engine(struct EngineConfig _config);
    ~Engine() = default;

    void Init();
    void StartFrame();
    void Update();
    void Render();
    void EndFrame();
    void Shutdown();

    void SetShouldRun(bool _b);
    bool ShouldRun();

    Logger* GetLoggerRef();
    Clock* GetClockRef();
private:
    Logger mLogger;
    Clock mClock;


    // Meta
    bool mEngineShouldRun = true;
    EngineConfig mConfig;

    // App
    WindowManager mWindowManager;
    SimulationManager mSimulationManager;
    Renderer mRenderer;
    UIManager mUIManager;

    std::vector<class Object*> mObjects;
    class Camera* mCamera;

};