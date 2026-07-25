#pragma once

#include <functional>

class UIManager{
public:
    using ExitCallback = std::function<void()>;
    using ToggleWindowModeCallback = std::function<void(int)>;

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
    void InitImGui(struct GLFWwindow* _window);
    void ShutdownImGui();

    void UI_TopBar();

public:

    void SetExitCallback(ExitCallback _callback);
    void SetToggleWindowModeCallback(ToggleWindowModeCallback _callback);
};