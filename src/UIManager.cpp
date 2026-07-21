#include "UIManager.h"
#include "TimeManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

void UIManager::Init(GLFWwindow* _window) 
{
    InitImGui(_window);
}

void UIManager::UIStartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}

void UIManager::UIDraw()
{
    //ImGui::ShowDemoWindow();
    UI_TopBar();
}

void UIManager::UIEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Shutdown()
{
    ShutdownImGui();
}

void UIManager::UpdateUI(float _dt)
{

}

void UIManager::InitImGui(GLFWwindow* _window)
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // GLFW backend, think it enables input for imgui but not sure.
    ImGui_ImplGlfw_InitForOpenGL(_window, true);

    // OpenGL backend
    ImGui_ImplOpenGL3_Init("#version 330");
}

void UIManager::ShutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
}

void UIManager::UI_TopBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        // File menu
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                if(mExitCallback)
                    mExitCallback();
            }

            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Window"))
        {
            if(ImGui::BeginMenu("Window Size"))
            {
                if (ImGui::MenuItem("FullScreen"))
                {
                    if(mToggleWindowModeCallback)
                        mToggleWindowModeCallback(1);
                }

                if (ImGui::MenuItem("Boardeless Fullscreen"))
                {
                    if(mToggleWindowModeCallback)
                        mToggleWindowModeCallback(2);
                }

                if (ImGui::MenuItem("Windowed"))
                {
                    if(mToggleWindowModeCallback)
                        mToggleWindowModeCallback(3);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        // FPS ---
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FPS: %.1f", TimeManager::GetFPS());
        float textWidth = ImGui::CalcTextSize(buffer).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textWidth - ImGui::GetStyle().ItemSpacing.x);
        ImGui::TextUnformatted(buffer);

        ImGui::EndMainMenuBar();
    }
}

void UIManager::SetExitCallback(ExitCallback _callback)
{
    mExitCallback = _callback;
} 

void UIManager::SetToggleWindowModeCallback(ToggleWindowModeCallback _callback)
{
    mToggleWindowModeCallback = _callback;
} 
