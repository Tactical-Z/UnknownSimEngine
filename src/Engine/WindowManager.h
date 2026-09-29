#pragma once

#include "glm/glm.hpp"

struct WindowColor{
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 1.f;
};

enum class WindowMode{
    WM_FULLSCREEN,
    WM_BOARDERLESS,
    WM_WINDOWED
};

struct WindowSettings
{
    const char* mWindowName;

    WindowColor mColor = {0.1f, 0.1f, 0.1f, 1.0f};

    int mWindoedWidth = 1280;
    int mWindowedHeight = 720;

    int mCurrentWidth = 1280;
    int mCurrentHeight = 720;

    int mWindowedPosX = 100;
    int mWindowedPosY = 100;

    WindowMode mWindowMode = WindowMode::WM_WINDOWED;
};

class WindowManager
{
public:
    using UpdateWindowSizeCallback = std::function<void(glm::ivec2)>;

    WindowManager() = default;
    ~WindowManager() = default;

    void Init(const char* _windowName, class Logger* _logger);
    void StartFrame();
    void Update(const float& _dt);
    void Render();
    void EndFrame();
    void Shutdown();

    // 1 full screen, 2 is boarderless fullscreen, 3 is windowed.
    void ToggleWindowMode(int _mode);
  
private:
    class Logger* mLogger = nullptr;
    UpdateWindowSizeCallback mUpdateWindowSizeCallback;
    struct GLFWwindow* mGLFWWindow = nullptr;
    WindowSettings mSettings;

    void PollEvents();
    void ClearGLBuffer();
    void SwapBuffers();

    void InitGLFW();
    bool InitWindow();

    void SetFullscreen();
    void SetBoarderlessFullscreen();
    void SetWindowed();

public:
    GLFWwindow* GetGLFWWindowPtr();
    WindowSettings* GetWindowSettingsPtr();
    bool WindowShouldClose();
    glm::ivec2 GetWindowSize();
    void SetUpdateWindowSizeCallback(UpdateWindowSizeCallback _callback);
};