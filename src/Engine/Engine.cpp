#include "Engine/Engine.h"
#include "Engine/Camera.h"
#include "Engine/Objects.h"

#include "glad/glad.h"

Engine::Engine(EngineConfig _config) : mConfig(_config)
{}

Engine::~Engine()
{
    delete mCamera;
    mCamera = nullptr;

    for(Object* object : mObjects){
        delete object;
    }
    mObjects.clear();
}

void Engine::Init()
{
    Logger::Set(&mLogger);

    // Objects
    mCamera = new Camera(glm::vec3(-5,0,0), 0, 0.f);
    mObjects.push_back(new BlackHole((gGridBoundsMin + gGridBoundsMax), 2.0f));
    // Window
    const char* windowName = "Black Hole Simulation";
    mWindowManager.Init(windowName);
    mWindowManager.SetUpdateWindowSizeCallback([this](glm::ivec2 _size){ mRenderer.SetWindowSize(_size); });
    // Simulation
    mSimulationManager.Init(mObjects);
    // UI
    mUIManager.Init(mWindowManager.GetGLFWWindowPtr());
    mUIManager.SetExitCallback([this](){ SetShouldRun(false); });
    mUIManager.SetToggleWindowModeCallback([this](int _mode){ mWindowManager.ToggleWindowMode(_mode); });
    mUIManager.SetGetCameraReferenceCallback([this](){ return mCamera; });
    mUIManager.SetGetSimulationSpeedRefrenceCallback([this]() -> float& { return mSimulationManager.GetSimulationSpeedRef();});
    mUIManager.SetGetPassTimeCallback([this]() -> std::vector<std::pair<const char*, float>> { return mSimulationManager.GetSimulationUIData();});
    // Renderer
    mRenderer.Init(mSimulationManager.GetRaytracerResources(), mCamera, &mObjects);
}

void Engine::StartFrame()
{   
    mWindowManager.PollEvents();
    mWindowManager.ClearGLBuffer();
    mUIManager.UIStartFrame();
}

void Engine::Update(float _dt)
{
    mSimulationManager.Update(_dt);
    mUIManager.UpdateUI(_dt);
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

