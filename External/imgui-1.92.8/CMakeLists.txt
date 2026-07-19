add_library(imgui
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp # Only for ImGui::ShowDemoWindow();
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
    
)

target_include_directories(imgui PUBLIC
    .
    backends
)