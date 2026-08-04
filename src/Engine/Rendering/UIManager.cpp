#include "UIManager.h"
#include "Engine/Camera.h"
#include "Util/STime.h"
#include "Util/Log.h"

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
    UI_CameraWindow();
    UI_WorldWindow();
    UI_SimulationWindow();
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

void UIManager::ToggleBool(bool& _b)
{
    _b = !_b;
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

            if (ImGui::MenuItem("Camera Window"))
            {
                ToggleBool(mEnableCameraWindow);
            }

            if (ImGui::MenuItem("World Window"))
            {
                ToggleBool(mEnableWorldWindow);
            }

            if (ImGui::MenuItem("Simulation Window"))
            {
                ToggleBool(mEnableSimulationWindow);
            }

            ImGui::EndMenu();
        }

        // FPS ---
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FPS: %.1f", STime::GetFPS());
        float textWidth = ImGui::CalcTextSize(buffer).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textWidth - ImGui::GetStyle().ItemSpacing.x);
        ImGui::TextUnformatted(buffer);

        ImGui::EndMainMenuBar();
    }
}

void UIManager::UI_CameraWindow()
{

    if(!mEnableCameraWindow || !mGetCameraReferenceCallback) return;

    Camera* cam = mGetCameraReferenceCallback();
    if(cam){
        if (ImGui::Begin("Camera")){
            
            //  --------- Position ---------

            glm::vec3 currentPosition = cam->GetPosition();
	        ImGui::Text("Position: ");
	        ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
	        ImGui::SameLine();
	        ImGui::SetNextItemWidth(mItemWidth);
	        ImGui::DragFloat("##PX", &currentPosition.x);
	        ImGui::SameLine();
	        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
	        ImGui::SameLine();
	        ImGui::SetNextItemWidth(mItemWidth);
	        ImGui::DragFloat("##PY", &currentPosition.y);
	        ImGui::SameLine();
	        ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
	        ImGui::SameLine();
	        ImGui::SetNextItemWidth(mItemWidth);
	        ImGui::DragFloat("##PZ", &currentPosition.z);
	        cam->SetPosition(currentPosition);

            //  --------- Rotation ---------
            
	        float currentPitch = cam->GetPitch();
	        float currentYaw = cam->GetYaw();
	        ImGui::Text("Rotation: ");
	        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pitch");
	        ImGui::SameLine();
	        ImGui::SetNextItemWidth(mItemWidth);
	        ImGui::DragFloat("##RX", &currentPitch);
                
	        ImGui::SameLine();
	        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Yaw ");
	        ImGui::SameLine();
	        ImGui::SetNextItemWidth(mItemWidth);
	        ImGui::DragFloat("##RY", &currentYaw);
	        cam->SetRotationPY(currentPitch, currentYaw);

            ImGui::End();
        }
    }
}

void UIManager::UI_WorldWindow()
{
    if(!mEnableWorldWindow) return;

    if(ImGui::Begin("World")){

                	
	    

        ImGui::End();
    }

}

void UIManager::UI_SimulationWindow()
{
    if(!mEnableSimulationWindow) return;

    if(ImGui::Begin("Simulation")){

        if(mGetSimulationSpeedRefrenceCallback){
            float& simSpeed = mGetSimulationSpeedRefrenceCallback();
            ImGui::Text("SimulationSpeed: ");
            ImGui::SameLine();
            ImGui::Text("%.2fx", simSpeed);
            ImGui::SameLine();
            if(simSpeed != 0){
                if(ImGui::Button("||")){
                    if(simSpeed != 0){
                        mOriginalSimSpeed = simSpeed;
                        simSpeed = 0.f;
                    }
                }
            } else {
                if(ImGui::Button("I>")){
                    if(mOriginalSimSpeed == 0.f){
                        simSpeed = 1.f;
                    } else {
                        simSpeed = mOriginalSimSpeed;
                        mOriginalSimSpeed = 0.f;
                    }

                }
            }
            ImGui::DragFloat("##RY", &simSpeed, 0.1);
        }

        ImGui::Text("Simulation compute time");
        std::vector<std::pair<const char*, float>> passTime = mPassTimeCallback();
        bool firstPipeline = true;
        for (const std::pair<const char*, float>& pass : passTime)
        {
            if (pass.second < 0.0f)
            {
                if (!firstPipeline)
                    ImGui::Unindent();
            
                ImGui::Text("%s", pass.first);
                ImGui::Indent();
            
                firstPipeline = false;
            }
            else
            {
                float frameTimeMS = STime::GetDeltaTime() * 1000.0f;
                // frame percent should be done with more care, currently the measurements are in cpu time and happen asyncronosly
                // that means we cannot compare cpu (dt frame time) with gpu opperation time.
                //float framePercent = (pass.second / frameTimeMS) * 100.0f;
                ImGui::Text("%s: %.2f ms", pass.first, pass.second/*, "(%.1f%%) frame time" framePercent*/);
            }
        }
        
        if (!firstPipeline)
            ImGui::Unindent();

        
        ImGui::End();
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

void UIManager::SetGetCameraReferenceCallback(GetCameraReferenceCallback _callback)
{
    mGetCameraReferenceCallback = _callback;
} 

void UIManager::SetGetSimulationSpeedRefrenceCallback(GetSimulationSpeedRefrenceCallback _callback)
{
    mGetSimulationSpeedRefrenceCallback = _callback;
}

void UIManager::SetGetPassTimeCallback(GetPassTimeCallback _callback)
{
    mPassTimeCallback = _callback;
}