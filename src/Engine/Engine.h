#pragma once

#include "Engine/EngineConfig.h"
#include "Engine/Logger.h"
#include "Engine/WindowManager.h"
#include "Engine/Camera.h"
#include "Engine/Objects.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/UIManager.h"

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
    Camera mCamera;
    WindowManager mWindowManager;
    UIManager mUIManager;
    std::vector<Object*> mObjects;
public:

    void SetShouldRun(bool _b);
    bool ShouldRun();
};