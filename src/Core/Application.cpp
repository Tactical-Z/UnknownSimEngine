
#include "Core/Application.h"
#include "Core/Threading/ThreadManager.h"
#include "Engine/Engine.h"
#include "Engine/EngineConfig.h"
#include "Util/TimeManager.h"
#include "Util/Log.h"

int Application::RunApp(){

    TimeManager::Init();

    EngineConfig config;
    config.mEnableWindow = true;
    config.mEnableRendere = true;
    config.mEnableWindow = true;

    Engine engine(config);
    engine.Init();

    while(engine.ShouldRun()){
        TimeManager::Update();
        engine.StartFrame();
        engine.Update(TimeManager::GetDeltaTime());
        engine.Render();
        engine.EndFrame();
    }

    // general approach:
    //// 1. Run compute shader
    //glUseProgram(computeProgram);
    //glDispatchCompute(...);
    //
    //// 2. Wait for compute to finish
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //
    //// 3. Draw fullscreen triangle
    //glUseProgram(displayProgram);
    //glBindTexture(GL_TEXTURE_2D, outputTexture);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    engine.Shutdown();

    return 0;
}
