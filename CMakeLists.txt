cmake_minimum_required(VERSION 3.20)

project(UnknownProject)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    main.cpp
    external/glad/src/glad.c
    src/Core/Application.cpp
    src/Core/Threading/ThreadManager.cpp
    src/Core/Threading/ThreadContext.cpp
    src/Engine/Engine.cpp
    src/Engine/Logger.cpp
    src/Engine/WindowManager.cpp
    src/Engine/Camera.cpp
    src/Engine/Objects.cpp
    src/Engine/Rendering/UIManager.cpp
    src/Engine/Rendering/Renderer.cpp
    src/Engine/Rendering/Texture.cpp
    src/Engine/Simulation/SimulationManager.cpp
    src/Engine/Simulation/Simulator.cpp
    src/Shaders/Shader.cpp
    src/Shaders/VisShader.cpp
    src/Shaders/ComputeShader.cpp
    src/Util/SMath.cpp
    src/Util/STime.cpp
)

set(HEADERS
    src/Core/Application.h
    src/Core/Threading/ThreadManager.h
    src/Core/Threading/ThreadContext.h
    src/Engine/Engine.h
    src/Engine/EngineConfig.h
    src/Engine/Logger.h
    src/Engine/WindowManager.h
    src/Engine/Camera.h
    src/Engine/Objects.h
    src/Engine/Rendering/UIManager.h
    src/Engine/Rendering/Renderer.h
    src/Engine/Rendering/Texture.h
    src/Engine/Simulation/SimulationManager.h
    src/Engine/Simulation/Simulator.h
    src/Engine/Simulation/ParticleData.h
    src/Engine/Simulation/SimData.h
    src/Engine/Simulation/HashGrid.h
    src/Shaders/Shader.h
    src/Shaders/VisShader.h
    src/Shaders/ComputeShader.h
    src/Util/SMath.h
    src/Util/STime.h
    src/Util/Log.h
    src/Util/Path.h
)

add_executable(UnknownProject
    ${SOURCES}
    ${HEADERS}
)

find_package(OpenGL REQUIRED)

target_include_directories(UnknownProject PRIVATE
    external/glad/include
    external/glm-1.0.3
    external/stb_i
    src
)

add_subdirectory(external/glfw-3.4)
add_subdirectory(external/imgui-1.92.8)
target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(UnknownProject
    OpenGL::GL
    glfw
    imgui
)

target_compile_definitions(UnknownProject PRIVATE
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets/"
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shader_src/"
    SRC_DIR="${CMAKE_SOURCE_DIR}/src/"
)