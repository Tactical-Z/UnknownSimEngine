#pragma once

#include <functional>
#include <utility>

class UIManager{
public:
    using ExitCallback = std::function<void()>;
    using ToggleWindowModeCallback = std::function<void(int)>;
    using GetCameraReferenceCallback = std::function<class Camera*()>;
    using GetSimulationSpeedRefrenceCallback = std::function<float&()>;
    using GetPassTimeCallback = std::function<std::vector<std::pair<const char*, float>>()>;

    UIManager() = default;
    ~UIManager() = default;

    void Init(struct GLFWwindow* _window);
    void UIStartFrame();
    void UIDraw();
    void UIEndFrame();
    void Shutdown();
    void UpdateUI(float _dt);

private:
    ExitCallback mExitCallback;
    ToggleWindowModeCallback mToggleWindowModeCallback;
    GetCameraReferenceCallback mGetCameraReferenceCallback;
    GetSimulationSpeedRefrenceCallback mGetSimulationSpeedRefrenceCallback;
    GetPassTimeCallback mPassTimeCallback;
    const float mItemWidth = 60.f;

    bool mEnableCameraWindow = true;
    bool mEnableWorldWindow = false;
    bool mEnableSimulationWindow = false;
    float mOriginalSimSpeed = 0.f;
    
    void InitImGui(struct GLFWwindow* _window);
    void ShutdownImGui();
    void ToggleBool(bool& _b);

    void UI_TopBar();
    void UI_CameraWindow();
    void UI_WorldWindow();
    void UI_SimulationWindow();

public:

    void SetExitCallback(ExitCallback _callback);
    void SetToggleWindowModeCallback(ToggleWindowModeCallback _callback);
    void SetGetCameraReferenceCallback(GetCameraReferenceCallback _callback);
    void SetGetSimulationSpeedRefrenceCallback(GetSimulationSpeedRefrenceCallback _callback);
    void SetGetPassTimeCallback(GetPassTimeCallback _callback);
};