#include "UIManager.h"
#include "engine/Camera.h"
#include "core/clock/Clock.h"
#include "util/AppUtil.h"

#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"

void UIManager::Init(GLFWwindow* _window, Logger* _logger, Clock* _clock) 
{   
    mLogger = _logger;
    mClock = _clock;
    InitImGui(_window);
}

void UIManager::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::Update(const float& _dt)
{

}

void UIManager::Render()
{
    UI_TopBar();
    UI_DockingWindow();
    
    UI_Viewport();
    UI_Camera();
    UI_SimulationData();
    UI_AppData();
    UI_Console();
    //ImGui::ShowDemoWindow();
}

void UIManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Shutdown()
{
    ShutdownImGui();
}

void UIManager::InitImGui(GLFWwindow* _window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    // GLFW backend, enables input and lings to imgui to glfw
    ImGui_ImplGlfw_InitForOpenGL(_window, true);

    // OpenGL backend
    ImGui_ImplOpenGL3_Init("#version 330");
}

void UIManager::ShutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
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

            if (ImGui::MenuItem(mName_Window_Viewport, nullptr, mEnable_Window_Viewport))
            {
                ToggleBool(&mEnable_Window_Viewport);
            }

            if (ImGui::MenuItem(mName_Window_Camera, nullptr, mEnable_Window_Camera))
            {
                ToggleBool(&mEnable_Window_Camera);
            }

            if (ImGui::MenuItem(mName_Window_SimData, nullptr, mEnable_Window_SimData))
            {
                ToggleBool(&mEnable_Window_SimData);
            }

            if (ImGui::MenuItem(mName_Window_AppData, nullptr, mEnable_Window_AppData))
            {
                ToggleBool(&mEnable_Window_AppData);
            }

            if (ImGui::MenuItem(mName_Window_Console, nullptr, mEnable_Window_Console))
            {
                ToggleBool(&mEnable_Window_Console);
            }

            ImGui::EndMenu();
        }

        // FPS ---
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Avg FPS: %.1f", mClock->GetAvgFPS());
        float textWidth = ImGui::CalcTextSize(buffer).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textWidth - ImGui::GetStyle().ItemSpacing.x);
        ImGui::TextUnformatted(buffer);

        ImGui::EndMainMenuBar();
    }
}

void UIManager::UI_DockingWindow()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("MainDockSpace", nullptr, windowFlags);

    ImGuiID dockspace_id = ImGui::GetID("DockSpace");

    ImGui::DockSpace(
        dockspace_id,
        ImVec2(0, 0),
        ImGuiDockNodeFlags_None
    );

    ImGui::End();

    ImGui::PopStyleVar(2);
}

void UIManager::UI_Viewport()
{
    if (!mEnable_Window_Viewport) 
        return;

    ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(mName_Window_Viewport, &mEnable_Window_Viewport))
    {
        ImGui::End();
        return;
    }

    float spacingx = ImGui::GetContentRegionAvail().x * mAppData_sectionWidth_scale;
    float spacingy = ImGui::GetContentRegionAvail().y * mAppData_sectionHeight_scale;
    ImVec2 spacing = ImVec2(spacingx, spacingy);

    // TODO: Viewport stuff goes here:

    ImGui::End();
}

void UIManager::UI_Camera()
{
    if (!mEnable_Window_Camera || !mGetCameraRefCallback) 
        return;

    ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(mName_Window_Camera, &mEnable_Window_Camera))
    {
        ImGui::End();
        return;
    }

    float spacingx = ImGui::GetContentRegionAvail().x * mAppData_sectionWidth_scale;
    float spacingy = ImGui::GetContentRegionAvail().y * mAppData_sectionHeight_scale;
    ImVec2 spacing = ImVec2(spacingx, spacingy);

    Camera* cam = mGetCameraRefCallback();
    if(cam){
        Section_Camera_Transform(spacing, cam);
    }

    ImGui::End();
}

void UIManager::Section_Camera_Transform(const ImVec2& _spacing, Camera* _cam)
{
    //  --------- Position ---------
    glm::vec3 currentPosition = _cam->GetPosition();
    ImGui::Text("Position: ");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(mItemWidth_Camera_Transform);
    ImGui::DragFloat("##PX", &currentPosition.x);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(mItemWidth_Camera_Transform);
    ImGui::DragFloat("##PY", &currentPosition.y);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(mItemWidth_Camera_Transform);
    ImGui::DragFloat("##PZ", &currentPosition.z);
    _cam->SetPosition(currentPosition);

    //  --------- Rotation ---------
    float currentPitch = _cam->GetPitch();
    float currentYaw = _cam->GetYaw();
    ImGui::Text("Rotation: ");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pitch");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(mItemWidth_Camera_Transform);
    ImGui::DragFloat("##RX", &currentPitch);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Yaw ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(mItemWidth_Camera_Transform);
    ImGui::DragFloat("##RY", &currentYaw);
    _cam->SetRotationPY(currentPitch, currentYaw);
}

void UIManager::UI_SimulationData()
{
    if (!mEnable_Window_SimData) 
        return;

    ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(mName_Window_SimData, &mEnable_Window_SimData))
    {
        ImGui::End();
        return;
    }

    float spacingx = ImGui::GetContentRegionAvail().x * mAppData_sectionWidth_scale;
    float spacingy = ImGui::GetContentRegionAvail().y * mAppData_sectionHeight_scale;
    ImVec2 spacing = ImVec2(spacingx, spacingy);
    Section_SimulationData_TimeControls(spacing);

    // TODO: decide result for this, not relly necesary??
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
            float frameTimeMS = mClock->GetDeltaTime() * 1000.0f;
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

void UIManager::Section_SimulationData_TimeControls(const ImVec2& _spacing)
{
    if(mGetCameraRefCallback){
        float& simSpeed = mGetSimulationSpeedRefCallback();
        ImGui::Text("SimulationSpeed: ");
        ImGui::SameLine();
        ImGui::Text("%.2fx", simSpeed);
        ImGui::SameLine();
        if(simSpeed != 0){
            if(ImGui::Button("||")){
                if(simSpeed != 0){
                    mOriginalSimSpeed_SimData_TimeControls = simSpeed;
                    simSpeed = 0.f;
                }
            }
        } else {
            if(ImGui::Button("I>")){
                if(mOriginalSimSpeed_SimData_TimeControls == 0.f){
                    simSpeed = 1.f;
                } else {
                    simSpeed = mOriginalSimSpeed_SimData_TimeControls;
                    mOriginalSimSpeed_SimData_TimeControls = 0.f;
                }
            }
        }
        ImGui::DragFloat("##RY", &simSpeed, 0.1);
    }
}

void UIManager::UI_AppData()
{
    if (!mEnable_Window_AppData) 
        return;

    ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(mName_Window_AppData, &mEnable_Window_AppData))
    {
        ImGui::End();
        return;
    }

    float spacingx = ImGui::GetContentRegionAvail().x * mAppData_sectionWidth_scale;
    float spacingy = ImGui::GetContentRegionAvail().y * mAppData_sectionHeight_scale;
    ImVec2 spacing = ImVec2(spacingx, spacingy);
    ImGui::TextDisabled("APPLICATION PERFORMANCE");
    ImGui::Spacing();
    Section_AppData_RamMemory(spacing);
    Section_AppData_FrameData(spacing);
    Section_AppData_CPUData(spacing);

    ImGui::End();
}

void UIManager::Section_AppData_RamMemory(const ImVec2& _spacing){
    
    float ramMB = AppUtil::App::GetAppRamUsageMB();
    RamMemory_PeakRam = Math::Util::fmax(RamMemory_PeakRam, ramMB);
    float dt = mClock->GetDeltaTime();
    Section_AppData_RamMemory_UpdateHistory(ramMB, dt);

    ImGui::Separator();
    ImGui::Text("Memory Data");
    ImGui::Text("RAM Memory Usage (MB)");
    ImGui::SameLine();
    ImGui::TextDisabled("(30 seconds)");
    ImGui::Spacing();

    if (ImPlot::BeginPlot("##RAMPlot", ImVec2(-1.0f, 160.0f), ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect))
    {
        // Axis:
        ImPlot::SetupAxes(
            "Time",
            "RAM (MB)",
            ImPlotAxisFlags_NoMenus,
            ImPlotAxisFlags_NoMenus
        );


        // X axis
        ImPlot::SetupAxisLimits(
            ImAxis_X1,
            Math::Util::fmax(0.0f, (RamMemory_Elapsed - RamMemory_HistoryInSeconds)),
            Math::Util::fmax(RamMemory_HistoryInSeconds, RamMemory_Elapsed),
            ImGuiCond_Always
        );

        // Y axis
        float yMin = 0;
        float yMax = RamMemory_PeakRam + (RamMemory_PeakRam * RamMemory_GraphPadding);
        yMin = Math::Util::fmin(0.0, yMin);
        ImPlot::SetupAxisLimits( ImAxis_Y1, yMin, yMax, ImGuiCond_Always);

        // Plot:
        if (RamMemory_Offset > 0)
        {
            ImPlot::PlotLine("RAM", &RamMemory_TimeHistory[RamMemory_Offset], &RamMemory_History[RamMemory_Offset], RamMemory_HistorySize - RamMemory_Offset);
            ImPlot::PlotLine("RAM", &RamMemory_TimeHistory[0], &RamMemory_History[0], RamMemory_Offset);
        }
        else
        {
            ImPlot::PlotLine("RAM", RamMemory_TimeHistory, RamMemory_History, RamMemory_HistorySize);
        }

        ImPlot::EndPlot();
    }

    ImGui::BeginGroup();
    ImGui::TextDisabled("CURRENT RAM");
    ImGui::Text("%.1f MB", ramMB);
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("PEAK RAM");
    ImGui::Text("%.1f MB", RamMemory_PeakRam);
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("HISTORY");
    ImGui::Text("%.0f sec", RamMemory_HistoryInSeconds);
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("UPDATE INTERVAL");
    ImGui::Text("%.2f sec", RamMemory_SampleInterval);
    ImGui::EndGroup();
}

void UIManager::Section_AppData_RamMemory_UpdateHistory(const float& _ramMB, const float& _dt){

    RamMemory_Elapsed += _dt;
    RamMemory_SampleTimer += _dt;

    if (RamMemory_SampleTimer >= RamMemory_SampleInterval)
    {
        RamMemory_SampleTimer -= RamMemory_SampleInterval;

        RamMemory_History[RamMemory_Offset] = _ramMB;
        RamMemory_TimeHistory[RamMemory_Offset] = RamMemory_Elapsed;

        RamMemory_Offset = (RamMemory_Offset + 1) % RamMemory_HistorySize;
    }
}

void UIManager::Section_AppData_FrameData(const ImVec2& _spacing){ 

    ImGui::Separator();
    ImGui::Text("Frame Data");
    ImGui::Spacing();
    ImGui::BeginGroup();
    ImGui::TextDisabled("AVG FRAMES PER SEC");
    ImGui::Text("%.0f Fps", mClock->GetAvgFPS());
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("AVG FRAME TIME");
    ImGui::Text("%.4f Ms", mClock->GetAvgFrameTimeMs());
    ImGui::EndGroup();
}

void UIManager::Section_AppData_CPUData(const ImVec2& _spacing){
    
    ImGui::Separator();
    ImGui::Text("CPU Data");
    ImGui::Spacing();
    ImGui::BeginGroup();
    ImGui::TextDisabled("AVG CPU USAGE");
    ImGui::Text("%.1f %%", mClock->GetAvgCPUUsage());
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("PHYSICAL CORE COUNT");
    ImGui::Text("%i", AppUtil::App::GetPhysicalCoreCount());
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("LOGICAL PROCESSOR COUNT");
    ImGui::Text("%i", AppUtil::App::GetLogicalProcessorCount());
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, _spacing.x);
    ImGui::BeginGroup();
    ImGui::TextDisabled("THREAD COUNT");
    ImGui::Text("%i", AppUtil::App::GetProcessThreadCount());
    ImGui::EndGroup();
}

void UIManager::UI_Console(){

    if (!mEnable_Window_Console) 
        return;
    
    ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(mName_Window_Console, &mEnable_Window_Console))
    {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Clear"))
    {
        mLogger->Clear<ImGuiSink>();
    }

    ImGui::Separator();

    ImGui::BeginChild(
        "ConsoleOutput",
        ImVec2(0.0f, -ImGui::GetFrameHeightWithSpacing()),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    if (auto* sink = mLogger->GetSink<ImGuiSink>())
    {
        for (const LogEntry& log : sink->GetEntries())
        {
            ImVec4 color = LogLevelToImColor(log.mLogLevel);
        
            ImGui::TextColored(
                color,
                "%s%s%s",
                Logger::LevelToChar(log.mLogLevel),
                Logger::ThreadToChar(&log.mThreadContext).c_str(),
                log.mMessage.c_str()
            );

            if (log.mLogLevel > LogLevel::LL_WARNING)
            {
                ImGui::TextDisabled(
                    "File: %s",
                    log.mSourceLocation.file_name()
                );

                ImGui::TextDisabled(
                    "Function: %s",
                    log.mSourceLocation.function_name()
                );

                ImGui::TextDisabled(
                    "Line/Col: %u:%u",
                    log.mSourceLocation.line(),
                    log.mSourceLocation.column()
                );
            }
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();


    ImGui::End();
}

void UIManager::SetExitCallback(CallbackVoidNull _callback)
{
    mExitCallback = _callback;
} 

void UIManager::SetToggleWindowModeCallback(CallbackVoidInt _callback)
{
    mToggleWindowModeCallback = _callback;
}

void UIManager::SetGetCameraReferenceCallback(CallbackCameraRefNull _callback)
{
    mGetCameraRefCallback = _callback;
} 

void UIManager::SetGetSimulationSpeedRefrenceCallback(CallbackFloatRefNull _callback)
{
    mGetSimulationSpeedRefCallback = _callback;
}

void UIManager::SetGetPassTimeCallback(CallbackVecPairCCharFloatNull _callback)
{
    mPassTimeCallback = _callback;
}

void UIManager::ToggleBool(bool* _b)
{
    *_b = !(*_b);
}

ImVec4 UIManager::LogLevelToImColor(LogLevel _lvl){
    
    ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);

    switch (_lvl) {
    case LogLevel::LL_INFO:
        break;
    case LogLevel::LL_SUCCESS:
        color = ImVec4(0.f, 1.f, 0.f, 1.f); // Green
        break;
    case LogLevel::LL_DEBUG:
 		color = ImVec4(0.3f, 0.3f, 1.f, 1.f); // Blue
        break;
    case LogLevel::LL_DEBUG_ASSERT:
 		color = ImVec4(0.3f, 0.3f, 1.f, 1.f); // Blue
        break;
 	case LogLevel::LL_WARNING:
 		color = ImVec4(1.f, 1.f, 0.f, 1.f); // Yellow
        break;
 	case LogLevel::LL_ERROR:
 		color = ImVec4(1.f, 0.6f, 0.f, 1.f); // Orange
        break;
    case LogLevel::LL_FATAL:
 		color = ImVec4(1.f, 0.f, 0.f, 1.f); // Red
        break;
    default:
        break;
 	}

    return color;
}