#pragma once

#include <functional>
#include "Util/SMath.h"

class Renderer
{
public:
    using BindSimulationStorageBufferCallback = std::function<void(int)>;

    Renderer() = default;
    ~Renderer();
    
    unsigned int mVAO;
    glm::ivec2 mWindowSize; // updated by window manager
    class Texture* mDisplayTexture = nullptr;
    class Texture* mSkyboxTexture = nullptr;
    class VisShader* mVisShader = nullptr;
    class ComputeShader* mComputeShader = nullptr;
    void Init();
    void Render(const class Camera* _camera, const std::vector<class Object*>& _objects);

private:
    BindSimulationStorageBufferCallback mBindSimulationStorageBufferCallback;

    void InitBuffers();
    void InitShaders();
    void InitTextures();
    void GenerateDisplayTexture();
    void BindCamera(const class Shader* _shader, const class Camera* _camera);
    void BindObjects(const class Shader* _shader, const std::vector<class Object*>& _objects);
    void BindTextures(const class Shader* _shader);
    void BindSimulations(const class Shader* _shader);
    void BindUniforms(const class Shader* _shader);
public:

    void SetWindowSize(glm::ivec2 _newWindowSize);
    void SetBindSimulationStorageBufferCallback(BindSimulationStorageBufferCallback _callback);
};