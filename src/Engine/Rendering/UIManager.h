#pragma once
#include "core/errorHandling/Log.h"
#include "core/AppData.h"

#include <functional>
#include <utility>
#include "imgui.h"

class UIManager{
public:
    UIManager() = default;
    ~UIManager() = default;

    void Init(struct GLFWwindow* _window, class Logger* _logger, class Clock* _clock);
    void StartFrame();
    void Update(const float& _dt);
    void Render();
    void EndFrame();
    void Shutdown();

    void SetExitCallback(CallbackVoidNull _callback);
    void SetToggleWindowModeCallback(CallbackVoidInt _callback);
    void SetGetCameraReferenceCallback(CallbackCameraRefNull _callback);
    void SetGetSimulationSpeedRefrenceCallback(CallbackFloatRefNull _callback);
    void SetGetPassTimeCallback(CallbackVecPairCCharFloatNull _callback);

private:
    // Util
    class Logger* mLogger = nullptr;
    class Clock* mClock = nullptr;

    // Callbacks
    CallbackVoidNull mExitCallback;
    CallbackVoidInt mToggleWindowModeCallback;
    CallbackCameraRefNull mGetCameraRefCallback;
    CallbackFloatRefNull mGetSimulationSpeedRefCallback;
    CallbackVecPairCCharFloatNull mPassTimeCallback;
    
    // Funcitions
    void InitImGui(struct GLFWwindow* _window);
    void ShutdownImGui();
    
    // Top Bar //
    void UI_TopBar();

    // Docking window//
    void UI_DockingWindow();

    // Viewport //
    const char* mName_Window_Viewport = "Viewport";
    bool mEnable_Window_Viewport = true;
    void UI_Viewport();

    // Camera //
    const char* mName_Window_Camera = "Camera";
    bool mEnable_Window_Camera = true;
    void UI_Camera();
    const float mItemWidth_Camera_Transform = 60.f;
    void Section_Camera_Transform(const ImVec2& _spacing, Camera* _cam);

    // Simulation Data //
    const char* mName_Window_SimData = "Simulation Data";
    bool mEnable_Window_SimData = true;
    void UI_SimulationData();
    float mOriginalSimSpeed_SimData_TimeControls = 0.f;
    void Section_SimulationData_TimeControls(const ImVec2& _spacing);

    // App Data //
    const char* mName_Window_AppData = "App data";
    bool mEnable_Window_AppData = true;
    float mAppData_sectionWidth_scale = 0.10f;
    float mAppData_sectionHeight_scale = 1.0f;
    void UI_AppData();
    static constexpr int RamMemory_HistorySize = 120; // num smaples
    static constexpr float RamMemory_SampleInterval = 0.25f; // in seconds
    static constexpr float RamMemory_HistoryInSeconds = RamMemory_HistorySize * RamMemory_SampleInterval; // seconds of history
    float RamMemory_History[RamMemory_HistorySize] = {};
    float RamMemory_TimeHistory[RamMemory_HistorySize] = {};
    int RamMemory_Offset = 0;
    float RamMemory_Elapsed = 0.0f;
    float RamMemory_SampleTimer = 0.0f;
    float RamMemory_PeakRam = 0.0f;
    float RamMemory_GraphPadding = 0.2f;
    void Section_AppData_RamMemory(const ImVec2& _spacing);
    void Section_AppData_RamMemory_UpdateHistory(const float& _ramMB, const float& _dt);
    void Section_AppData_FrameData(const ImVec2& _spacing);
    void Section_AppData_CPUData(const ImVec2& _spacing);

    // Console //
    const char* mName_Window_Console = "Console";
    bool mEnable_Window_Console = true;
    void UI_Console();

    void ToggleBool(bool* _b);
    ImVec4 LogLevelToImColor(LogLevel _lvl);
};