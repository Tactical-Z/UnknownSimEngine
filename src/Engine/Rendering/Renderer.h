#pragma once

#include "Engine/Camera.h"
#include "Engine/Objects.h"
#include "Shaders/VisShader.h"
#include "Shaders/ComputeShader.h"
#include "Texture.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
      

    unsigned int mVAO;
    glm::ivec2 mWindowSize; // updated by window manager
    Texture* mDisplayTexture = nullptr;
    VisShader* mVisShader = nullptr;
    ComputeShader* mComputeShader = nullptr;
    void Init();
    void Render(const Camera& _camera, const std::vector<class Object*>& _objects);

private:

    void InitBuffers();
    void InitShaders();
    void GenerateTexture();
    void BindCamera(const Shader* _shader, const Camera& _camera);
    void BindObjects(const Shader* _shader, const std::vector<class Object*>& _objects);
public:

    void SetWindowSize(glm::ivec2 _newWindowSize);

};