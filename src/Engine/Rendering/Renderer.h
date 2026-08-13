#pragma once

#include "Util/SMath.h"
#include <vector>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    
    unsigned int mVAO;
    glm::ivec2 mWindowSize; // updated by window manager
    class Texture* mDisplayTexture = nullptr;
    class Texture* mSkyboxTexture = nullptr;
    class VisShader* mVisShader = nullptr;
    class SimulationPass* mRaytracePass = nullptr;

    const class Camera* mCameraRef;
    const std::vector<class Object*>* mObjectsRef;

    void Init(std::vector<struct SSBOBinding> _raytracerResources, const Camera* _camera, const std::vector<class Object*>* _objects);
    void Render();

private:
    void InitBuffers();
    void InitShaders(std::vector<struct SSBOBinding> _raytracerResources);
    void InitTextures();
    void GenerateDisplayTexture();
    void BindCamera(const class Shader* _shader);
    void BindReferenceObjects(const class Shader* _shader);
    void BindTextures(const class Shader* _shader);
    void BindUniforms(const class Shader* _shader);
public:

    void SetWindowSize(glm::ivec2 _newWindowSize);
};