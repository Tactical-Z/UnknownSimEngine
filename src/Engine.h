#pragma once

#include "EngineConfig.h"
#include "Logger.h"
#include "WindowManager.h"
#include "Renderer.h"
#include "UIManager.h"

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

public:

    void SetShouldRun(bool _b);
    bool ShouldRun();

};