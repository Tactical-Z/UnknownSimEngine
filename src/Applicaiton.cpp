
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
