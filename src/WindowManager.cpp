#include "WindowManager.h"
#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void FramebufferResizeCallback(GLFWwindow* _window, int _width, int _height)
{
    glViewport(0, 0, _width, _height);

    WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
    windowManager->GetWindowSettingsPtr()->mCurrentWidth = _width;
    windowManager->GetWindowSettingsPtr()->mCurrentHeight = _height;
}

void WindowManager::Init(const char* _windowName)
{
    InitGLFW(); // glfw should be engnie wide not window wide. 
    mSettings.mWindowName = _windowName;
    InitWindow();
};

void WindowManager::Shutdown()
{
    if(mGLFWWindow){
        glfwDestroyWindow(mGLFWWindow);
        mGLFWWindow = nullptr;
    }
    
    glfwTerminate(); // glfw technically not only window dependent, lifetime should be engine wide not window wide.
};

void WindowManager::PollEvents(){
    glfwPollEvents();
}

void WindowManager::ClearGLBuffer()
{
    glClearColor(
        mSettings.mColor.r, 
        mSettings.mColor.g, 
        mSettings.mColor.b, 
        mSettings.mColor.a
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowManager::SwapBuffers(){
    glfwSwapBuffers(mGLFWWindow);
}

void WindowManager::ToggleWindowMode(int _mode){
    switch (_mode)
    {
    case 1:
        SetFullscreen();
        break;
    case 2:
        SetBoarderlessFullscreen();
        break;
    case 3:
        SetWindowed();
        break;
    default:
        LOG_WARNING("Set window Mode failed, mode: '%i' does not exist", _mode);
        break;
    }
}

void WindowManager::InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool WindowManager::InitWindow()
{
    mGLFWWindow = glfwCreateWindow(
        mSettings.mWindoedWidth,
        mSettings.mWindowedHeight,
        mSettings.mWindowName,
        nullptr,
        nullptr
    );

    if(!mGLFWWindow)
    {
        LOG_ERROR("Failed to create GLFW window");
        return false;
    }

    glfwMakeContextCurrent(mGLFWWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD");
        return false;
    }

    glfwSetWindowUserPointer(mGLFWWindow, this);

    glfwSetFramebufferSizeCallback(mGLFWWindow, FramebufferResizeCallback);

    return true;
}

GLFWwindow* WindowManager::GetGLFWWindowPtr()
{
    return mGLFWWindow;
}

WindowSettings* WindowManager::GetWindowSettingsPtr()
{
    return &mSettings;
}

bool WindowManager::WindowShouldClose()
{
    return glfwWindowShouldClose(mGLFWWindow);
}

void WindowManager::SetFullscreen()
{
    if(mSettings.mWindowMode == WindowMode::WM_FULLSCREEN)
        return;
    mSettings.mWindowMode = WindowMode::WM_FULLSCREEN;

    glfwSetWindowAttrib(
        mGLFWWindow,
        GLFW_DECORATED,
        GLFW_TRUE
    );


    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(
        mGLFWWindow,
        monitor,
        0,
        0,
        mode->width,
        mode->height,
        mode->refreshRate
    );
}

void WindowManager::SetBoarderlessFullscreen()
{
    if(mSettings.mWindowMode == WindowMode::WM_BOARDERLESS)
        return;
    mSettings.mWindowMode = WindowMode::WM_BOARDERLESS;

    glfwSetWindowAttrib(
        mGLFWWindow,
        GLFW_DECORATED,
        GLFW_FALSE
    );

    glfwSetWindowMonitor(
        mGLFWWindow,
        nullptr,
        0,
        0,
        glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
        glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
        0
    );

}

void WindowManager::SetWindowed()
{
    if(mSettings.mWindowMode == WindowMode::WM_WINDOWED)
        return;
    mSettings.mWindowMode = WindowMode::WM_WINDOWED;

    glfwSetWindowAttrib(
        mGLFWWindow,
        GLFW_DECORATED,
        GLFW_TRUE
    );

    glfwSetWindowMonitor(
        mGLFWWindow,
        nullptr,
        mSettings.mWindowedPosX,
        mSettings.mWindowedPosY,
        mSettings.mWindoedWidth,
        mSettings.mWindowedHeight,
        0
    );
}

