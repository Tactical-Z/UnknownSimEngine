#include "Engine.h"

Engine::Engine(EngineConfig _config) : mConfig(_config)
{}

Engine::~Engine()
{
    for(Object* object : mObjects){
        delete object;
    }
    mObjects.clear();
}

void Engine::Init()
{
    Logger::Set(&mLogger);

    const char* windowName = "SimStuff";
    mWindowManager.Init(windowName);
    mWindowManager.SetUpdateWindowSizeCallback([this](glm::ivec2 _size){ mRenderer.SetWindowSize(_size); });
    mUIManager.Init(mWindowManager.GetGLFWWindowPtr());
    mUIManager.SetExitCallback([this](){ SetShouldRun(false); });
    mUIManager.SetToggleWindowModeCallback([this](int _mode){ mWindowManager.ToggleWindowMode(_mode); });
    mRenderer.Init();
    mCamera.Init();
    mObjects.push_back(new BlackHole(glm::vec3(1.0f, 0.0f, 0.0f), 0.1f));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void Engine::StartFrame()
{   
    mWindowManager.PollEvents();
    mWindowManager.ClearGLBuffer();
    mUIManager.UIStartFrame();
}

void Engine::Update(float _dt)
{
    mUIManager.UpdateUI(_dt);
}

void Engine::Render()
{
    mRenderer.Render(mCamera, mObjects);
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

