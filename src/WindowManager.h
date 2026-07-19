#pragma once

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
    WindowManager() = default;
    ~WindowManager() = default;

    void Init(const char* _windowName);
    void Shutdown();
    void PollEvents();
    void ClearGLBuffer();
    void SwapBuffers();

    // 1 full screen, 2 is boarderless fullscreen, 3 is windowed.
    void ToggleWindowMode(int _mode);
  
private:

    struct GLFWwindow* mGLFWWindow = nullptr;
    WindowSettings mSettings;

    void InitGLFW();
    bool InitWindow();

    void SetFullscreen();
    void SetBoarderlessFullscreen();
    void SetWindowed();

public:
    GLFWwindow* GetGLFWWindowPtr();
    WindowSettings* GetWindowSettingsPtr();
    bool WindowShouldClose();
};