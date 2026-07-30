#pragma once

#include "Engine/EngineConfig.h"
#include "Engine/Logger.h"
#include "Engine/WindowManager.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/UIManager.h"
#include "Engine/Simulation/SimulationManager.h"

//Todo: add error detection for glFunctions

class Engine
{
public:
    Engine() = default;
    Engine(struct EngineConfig _config);
    ~Engine();

    void Init();

    void StartFrame();
    void Update(float _dt);
    void Render();
    void EndFrame();
    void Shutdown();

private:

    bool mEngineShouldRun = true;
    EngineConfig mConfig;
    Logger mLogger;
    Renderer mRenderer;
    
    WindowManager mWindowManager;
    UIManager mUIManager;
    SimulationManager mSimulationManager;
    std::vector<class Object*> mObjects;
    class Camera* mCamera;
public:

    void SetShouldRun(bool _b);
    bool ShouldRun();
};