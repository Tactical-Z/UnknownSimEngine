#include "Engine/Rendering/Renderer.h"
#include "Engine/Camera.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/VisShader.h"
#include "Engine/Rendering/Texture.h"
#include "Engine/Camera.h"
#include "Engine/Objects.h"

#include "Util/STime.h"
#include "Util/Path.h"
#include "Util/Log.h"

#include <glad/glad.h>

Renderer::~Renderer(){
    delete mDisplayTexture;
    delete mVisShader;
    delete mSkyboxTexture;
    delete mComputeShader;
}

void Renderer::Init()
{
    InitBuffers();
    InitShaders();
    InitTextures();
}

void Renderer::Render(const Camera* _camera, const std::vector<class Object*>& _objects)
{
    if(!mDisplayTexture || !mVisShader || !mComputeShader)
        return;

    
    // Compute Shader
    mComputeShader->use();
    BindCamera(mComputeShader, _camera);
    BindObjects(mComputeShader, _objects);
    BindTextures(mComputeShader);
    BindSimulations(mComputeShader);
    BindUniforms(mComputeShader);

    GLuint groupsX = (mWindowSize.x + 9) / 10;
    GLuint groupsY = (mWindowSize.y + 9) / 10;
    glDispatchCompute(groupsX, groupsY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    // Render Shader
    glBindVertexArray(mVAO);
    mVisShader->use();
    mDisplayTexture->Bind(0, TextureType::TT_SAMPLER2D);
    mVisShader->setInt("textureSampler", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::InitBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
}

void Renderer::InitShaders()
{
    mVisShader = new VisShader(PathUtil::shader_dir("general.vert"), PathUtil::shader_dir("general.frag"));
    mComputeShader = new ComputeShader(PathUtil::shader_dir("general.comp"));
}

void Renderer::InitTextures()
{
    mSkyboxTexture = new Texture(PathUtil::skybox_files(PathUtil::asset_dir("skybox/blue/")));
    GenerateDisplayTexture();
}

void Renderer::GenerateDisplayTexture()
{
    delete mDisplayTexture;
    mDisplayTexture = nullptr;
    mDisplayTexture = new Texture(mWindowSize.x, mWindowSize.y);
}

void Renderer::BindCamera(const Shader* _shader, const Camera* _camera)
{
    _shader->use();
    _shader->setVec3("camera.position", _camera->GetPosition());
    _shader->setVec3("camera.up", _camera->GetUp());
    _shader->setVec3("camera.right", _camera->GetRight());
    _shader->setVec3("camera.front", _camera->GetFront());
    _shader->setFloat("camera.fov", _camera->GetFov());
    _shader->setFloat("camera.maxRayLength", _camera->GetFarplane());
    _shader->setInt("camera.numRayChecks", _camera->GetNumRayChecks());
}

void Renderer::BindObjects(const Shader* _shader, const std::vector<Object*>& _objects)
{
    _shader->use();
    for (Object* object : _objects){
        BlackHole* blackHole = dynamic_cast<BlackHole*>(object);
        if(blackHole){
            _shader->setVec3("sphere.position", blackHole->GetPosition());
            _shader->setFloat("sphere.radius", blackHole->GetRadius());
        }
        else {
            LOG_WARNING("No Black hole detected");
        }
    }
}

void Renderer::BindTextures(const class Shader* _shader)
{
    // ToDo: Upgrade all bindings to support different shader configurations, (vis vs compute)
    _shader->use();
    mDisplayTexture->Bind(0, TextureType::TT_IMAGE2D);
    mSkyboxTexture->Bind(0, TextureType::TT_SAMPLERCUBE);
    _shader->setInt("skybox", 0);
}

void Renderer::BindSimulations(const class Shader* _shader)
{
    if(mBindParticleStorageBufferCallback)
        mBindParticleStorageBufferCallback();
    else 
        LOG_ERROR("Bind Simulation Buffer Callback invalid when called.");
}

void Renderer::BindUniforms(const class Shader* _shader)
{
    
}

void Renderer::SetWindowSize(glm::ivec2 _newWindowSize){
    
    mWindowSize = _newWindowSize;
    GenerateDisplayTexture();
}

void Renderer::SetBindParticleStorageBufferCallback(BindParticleStorageBufferCallback _callback)
{
    mBindParticleStorageBufferCallback = _callback;
}