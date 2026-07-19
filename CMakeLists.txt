cmake_minimum_required(VERSION 3.20)

project(UnknownProject)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    main.cpp
    external/glad/src/glad.c
    src/Applicaiton.cpp
    src/Engine.cpp
    src/Logger.cpp
    src/ThreadManager.cpp
    src/ThreadContext.cpp
    src/WindowManager.cpp
    src/Renderer.cpp
    src/UIManager.cpp
)

set(HEADERS
    src/Applicaiton.h
    src/Engine.h
    src/EngineConfig.h
    src/Logger.h
    src/Log.h
    src/ThreadManager.h
    src/ThreadContext.h
    src/WindowManager.h
    src/Renderer.h
    src/UIManager.h
)

add_executable(UnknownProject
    ${SOURCES}
    ${HEADERS}
)

find_package(OpenGL REQUIRED)

target_include_directories(UnknownProject PRIVATE
    external/glad/include
    external/glm-1.0.3
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