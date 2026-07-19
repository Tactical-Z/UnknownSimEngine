
#include "Applicaiton.h"
#include "Engine.h"
#include "EngineConfig.h"
#include "ThreadManager.h"
#include "Log.h"

int Applicaiton::RunApp(){

    EngineConfig config;
    config.mEnableWindow = true;
    config.mEnableRendere = true;
    config.mEnableWindow = true;

    Engine engine(config);
    engine.Init();

    while(engine.ShouldRun()){
        engine.StartFrame();
        engine.Update(1.f);
        engine.Render();
        engine.EndFrame();
    }
    
    engine.Shutdown();

    return 0;
}
