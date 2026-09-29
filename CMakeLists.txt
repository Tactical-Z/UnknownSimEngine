cmake_minimum_required(VERSION 3.20)

project(UnknownProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for logger __VA_OPT__
if(MSVC)
    add_compile_options(/Zc:preprocessor)
endif()


set(SOURCES
    main.cpp
    external/glad/src/glad.c
    src/core/Application.cpp
    src/core/clock/Clock.cpp
    src/core/errorHandling/Logger.cpp
    src/core/errorHandling/LogSink.cpp
    src/core/threading/ThreadManager.cpp
    src/core/threading/ThreadContext.cpp
    src/engine/Engine.cpp
    src/engine/WindowManager.cpp
    src/engine/Camera.cpp
    src/engine/Objects.cpp
    src/engine/rendering/UIManager.cpp
    src/engine/rendering/Renderer.cpp
    src/engine/rendering/Texture.cpp
    src/engine/simulation/SimulationManager.cpp
    src/engine/simulation/SimulationPipeline.cpp
    src/engine/simulation/SimulationPass.cpp
    src/shaders/Shader.cpp
    src/shaders/VisShader.cpp
    src/shaders/ComputeShader.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/AppData.h
    src/core/clock/Clock.h
    src/core/errorHandling/Log.h
    src/core/errorHandling/LogEntry.h
    src/core/errorHandling/Logger.h
    src/core/errorHandling/LogLevel.h
    src/core/errorHandling/LogSink.h
    src/core/threading/ThreadManager.h
    src/core/threading/ThreadContext.h
    src/engine/Engine.h
    src/engine/EngineConfig.h
    src/engine/WindowManager.h
    src/engine/Camera.h
    src/engine/Objects.h
    src/engine/rendering/UIManager.h
    src/engine/rendering/Renderer.h
    src/engine/rendering/Texture.h
    src/engine/simulation/SimulationManager.h
    src/engine/simulation/SimulationPipeline.h
    src/engine/simulation/SimulationPass.h
    src/engine/simulation/SimData.h
    src/shaders/Shader.h
    src/shaders/VisShader.h
    src/shaders/ComputeShader.h
    src/Util/AppUtil.h
    src/Util/Math.h
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
add_library(implot STATIC
    external/implot-1.0/implot.cpp
    external/implot-1.0/implot_items.cpp
)

target_link_libraries(imgui PRIVATE glfw)
target_include_directories(implot PUBLIC
    external/implot-1.0
    external/imgui-1.92.8
)

target_link_libraries(UnknownProject
    OpenGL::GL
    glfw
    imgui
    implot
)

target_compile_definitions(UnknownProject PRIVATE
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets/"
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shader_src/"
    SRC_DIR="${CMAKE_SOURCE_DIR}/src/"
)