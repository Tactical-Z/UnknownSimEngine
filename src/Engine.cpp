#include "Engine.h"

Engine::Engine(EngineConfig _config) : mConfig(_config)
{}

Engine::~Engine()
{}

void Engine::Init()
{
    Logger::Set(&mLogger);

    const char* windowName = "SimStuff";
    mWindowManager.Init(windowName);
    mUIManager.Init(mWindowManager.GetGLFWWindowPtr());
    mUIManager.SetExitCallback([this](){ SetShouldRun(false); });
    mUIManager.SetToggleWindowModeCallback([this](int _mode){ mWindowManager.ToggleWindowMode(_mode); });
    mRenderer.Init();
}

void Engine::StartFrame()
{   
    mWindowManager.PollEvents();
    mWindowManager.ClearGLBuffer();
    mUIManager.UIStartFrame();
}

void Engine::Update(float _dt)
{

}

void Engine::Render()
{
    mRenderer.Render();
    mUIManager.UIDraw();
}

void Engine::EndFrame()
{

    mUIManager.UIEndFrame();
    mWindowManager.SwapBuffers();
}

void Engine::Shutdown()
{
    Logger::Set(nullptr);

    mWindowManager.Shutdown();
    mUIManager.Shutdown();
    
}

void Engine::SetShouldRun(bool _b)
{
    mEngineShouldRun = _b;
}

bool Engine::ShouldRun()
{
    return mEngineShouldRun && !mWindowManager.WindowShouldClose();
}