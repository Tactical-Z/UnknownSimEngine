#include "Engine.h"

Engine::Engine(EngineConfig _config) : mConfig(_config)
{}

Engine::~Engine()
{}

void Engine::Init()
{
    Logger::Set(&mLogger);

    if(mConfig.mEnableWindow){
        const char* windowName = "SimStuff";
        mWindowManager.Init(windowName);
        mUIManager.Init(mWindowManager.GetGLFWWindowPtr());
        mUIManager.SetExitCallback([this](){ SetShouldRun(false); });
        mUIManager.SetToggleWindowModeCallback([this](int _mode){ mWindowManager.ToggleWindowMode(_mode); }
    );

        if(mConfig.mEnableInput){
            // input (maybe in window manager or command class)
        }

        if(mConfig.mEnableRendere){
            // renderer
        }
    }
}


void Engine::StartFrame()
{   
    if(mConfig.mEnableWindow){
        mWindowManager.PollEvents();
        mWindowManager.ClearGLBuffer();
        mUIManager.UIStartFrame();
    }
    
}

void Engine::Update(float _dt)
{

}

void Engine::Render()
{
    mUIManager.UIDraw();
}

void Engine::EndFrame()
{
    if(mConfig.mEnableWindow){
        mUIManager.UIEndFrame();
        mWindowManager.SwapBuffers();
    }
}

void Engine::Shutdown()
{
    Logger::Set(nullptr);

    if(mConfig.mEnableWindow){
        mWindowManager.Shutdown();
        mUIManager.Shutdown();
        
        if(mConfig.mEnableInput){
            // input 
        }

        if(mConfig.mEnableRendere){
            // renderer
        }
    }
}

void Engine::SetShouldRun(bool _b)
{
    mEngineShouldRun = _b;
}

bool Engine::ShouldRun()
{
    return mEngineShouldRun && !mWindowManager.WindowShouldClose();
}