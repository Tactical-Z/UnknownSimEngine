#include "engine/Engine.h"
#include "engine/Camera.h"
#include "engine/Objects.h"

#include "glad/glad.h"

Engine::Engine(EngineConfig _config) : mConfig(_config), mLogger(), mClock(&mLogger)
{}

void Engine::Init()
{
    // Util
    Logger* loggerRef = GetLoggerRef();
    Clock* clockRef = GetClockRef();
    loggerRef->AddSink(new ConsoleSink());
    loggerRef->AddSink(new ImGuiSink());

    // Objects
    mCamera = new Camera(glm::vec3(-24,0,24), -45.f, 0.f);
    mObjects.push_back(new BlackHole((gGridBoundsMin + gGridBoundsMax), 2.0f));
    
    // Managers --
    // Window
    const char* windowName = "Black Hole Simulation";
    mWindowManager.Init(windowName, loggerRef);
    mWindowManager.SetUpdateWindowSizeCallback([this](glm::ivec2 _size){ mRenderer.SetWindowSize(_size); });
    // Simulation
    mSimulationManager.Init(mObjects, loggerRef, clockRef);
    // Renderer
    mRenderer.Init(loggerRef, clockRef, mSimulationManager.GetRaytracerResources(), mCamera, &mObjects);
    // UI
    mUIManager.Init(mWindowManager.GetGLFWWindowPtr(), loggerRef, clockRef);
    mUIManager.SetExitCallback([this](){ SetShouldRun(false); });
    mUIManager.SetToggleWindowModeCallback([this](int _mode){ mWindowManager.ToggleWindowMode(_mode); });
    mUIManager.SetGetCameraReferenceCallback([this](){ return mCamera; });
    mUIManager.SetGetSimulationSpeedRefrenceCallback([this]() -> float& { return mSimulationManager.GetSimulationSpeedRef();});
    mUIManager.SetGetPassTimeCallback([this]() -> std::vector<std::pair<const char*, float>> { return mSimulationManager.GetSimulationUIData();});
}

void Engine::StartFrame()
{   
    GetClockRef()->Update();
    mWindowManager.StartFrame(); // First
    mSimulationManager.StartFrame();
    mRenderer.StartFrame();
    mUIManager.StartFrame();
}

void Engine::Update()
{
    float dt = GetClockRef()->GetDeltaTime();
    mWindowManager.Update(dt);
    mSimulationManager.Update(dt);
    mRenderer.Update(dt);
    mUIManager.Update(dt);
}

void Engine::Render()
{
    mWindowManager.Render();
    mSimulationManager.Render();
    mRenderer.Render();
    mUIManager.Render(); // not uidraw()
}

void Engine::EndFrame()
{
    mUIManager.EndFrame();
    mRenderer.EndFrame();
    mSimulationManager.EndFrame();
    mWindowManager.EndFrame(); // Last
}

void Engine::Shutdown()
{
    // managers
    mWindowManager.Shutdown();
    mSimulationManager.Shutdown();
    mRenderer.Shutdown();
    mUIManager.Shutdown();

    // Other
    delete mCamera;
    mCamera = nullptr;

    for(Object* object : mObjects){
        delete object;
    }
    mObjects.clear();
}

void Engine::SetShouldRun(bool _b)
{
    mEngineShouldRun = _b;
}

bool Engine::ShouldRun()
{
    return mEngineShouldRun && !mWindowManager.WindowShouldClose();
}

Logger* Engine::GetLoggerRef()
{
    return &mLogger;
}

Clock* Engine::GetClockRef()
{
    return &mClock;
}