
#include "core/Application.h"
#include "core/Threading/ThreadManager.h"
#include "engine/Engine.h"
#include "engine/EngineConfig.h"

int Application::RunApp(){

    EngineConfig config;
    config.mEnableWindow = true;
    config.mEnableRendere = true;
    config.mEnableWindow = true;

    Engine engine(config);
    engine.Init();

    while(engine.ShouldRun()){
        engine.StartFrame();
        engine.Update();
        engine.Render();
        engine.EndFrame();
    }

    engine.Shutdown();

    return 0;
}
