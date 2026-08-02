#pragma once

#include <functional>

class UIManager{
public:
    using ExitCallback = std::function<void()>;
    using ToggleWindowModeCallback = std::function<void(int)>;
    using GetCameraReferenceCallback = std::function<class Camera*()>;
    using GetSimulationSpeedRefrenceCallback = std::function<float&()>;

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
    const float mItemWidth = 60.f;

    bool mEnableCameraWindow = false;
    bool mEnableWorldWindow = true;
    float mOriginalSimSpeed = 0.f;
    
    void InitImGui(struct GLFWwindow* _window);
    void ShutdownImGui();
    void ToggleBool(bool& _b);

    void UI_TopBar();
    void UI_CameraWindow();
    void UI_WorldWindow();

public:

    void SetExitCallback(ExitCallback _callback);
    void SetToggleWindowModeCallback(ToggleWindowModeCallback _callback);
    void SetGetCameraReferenceCallback(GetCameraReferenceCallback _callback);
    void SetGetSimulationSpeedRefrenceCallback(GetSimulationSpeedRefrenceCallback _callback);
};