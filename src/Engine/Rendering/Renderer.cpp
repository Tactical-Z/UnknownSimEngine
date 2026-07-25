#include "Renderer.h"
#include "Engine/Camera.h"

#include "Util/TimeManager.h"
#include "Util/Path.h"
#include "Util/Log.h"

#include <glad/glad.h>

Renderer::~Renderer(){
    delete mDisplayTexture;
    delete mVisShader;
    delete mComputeShader;
}

void Renderer::Init()
{
    InitBuffers();
    InitShaders();
}

void Renderer::Render(const Camera& _camera, const std::vector<class Object*>& _objects)
{
    if(!mDisplayTexture || !mVisShader || !mComputeShader)
        return;

    // Compute Shader
    BindCamera(mComputeShader, _camera);
    BindObjects(mComputeShader, _objects);
    mComputeShader->use();

    mComputeShader->setFloat("time", TimeManager::GetTimeSinceAppStart());
    mDisplayTexture->useCustomTex();
    glDispatchCompute((unsigned int)mWindowSize.x/10, (unsigned int)mWindowSize.y/10, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    // Render Shader
    glBindVertexArray(mVAO);
    mVisShader->use();
    mDisplayTexture->use();
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

void Renderer::GenerateTexture()
{
    delete mDisplayTexture;
    mDisplayTexture = nullptr;
    mDisplayTexture = new Texture(mWindowSize.x, mWindowSize.y);
}

void Renderer::BindCamera(const Shader* _shader, const Camera& _camera)
{
    _shader->use();
    _shader->setVec3("camera.position", _camera.GetPosition());
    _shader->setVec3("camera.up", _camera.GetUp());
    _shader->setVec3("camera.right", _camera.GetRight());
    _shader->setVec3("camera.front", _camera.GetFront());
    _shader->setFloat("camera.fov", _camera.GetFov());
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

void Renderer::SetWindowSize(glm::ivec2 _newWindowSize){
    
    mWindowSize = _newWindowSize;
    GenerateTexture();
}