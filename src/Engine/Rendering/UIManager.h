#pragma once

#include <functional>

class UIManager{
public:
    using ExitCallback = std::function<void()>;
    using ToggleWindowModeCallback = std::function<void(int)>;
    using GetCameraReferenceCallback = std::function<class Camera*()>;

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

    const float mItemWidth = 60.f;

    bool mEnableCameraWindow = false;
    
    void InitImGui(struct GLFWwindow* _window);
    void ShutdownImGui();
    void ToggleBool(bool& _b);

    void UI_TopBar();
    void UI_CameraWindow();

public:

    void SetExitCallback(ExitCallback _callback);
    void SetToggleWindowModeCallback(ToggleWindowModeCallback _callback);
    void SetGetCameraReferenceCallback(GetCameraReferenceCallback _callback);
};