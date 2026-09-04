#include "Engine/Rendering/Renderer.h"
#include "Engine/Camera.h"
#include "Engine/Simulation/SimData.h"
#include "Engine/Simulation/SimulationPass.h"
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
    delete mRaytracePass;

    // Not owned by this class
    mCameraRef = nullptr;
    mObjectsRef = nullptr;
}

void Renderer::Init(std::vector<SSBOBinding> _raytracerResources, const Camera* _camera, const std::vector<class Object*>* _objects)
{
    mCameraRef = _camera;
    mObjectsRef = _objects;

    InitBuffers();
    InitShaders(_raytracerResources);
    InitTextures();
}

void Renderer::Render()
{
    if(!mDisplayTexture || !mVisShader || !mRaytracePass)
        return;

    // Compute Shader
    mRaytracePass->Execute();
    
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

void Renderer::InitShaders(std::vector<SSBOBinding> _raytracerResources)
{
    mVisShader = new VisShader(PathUtil::shader_dir("general.vert"), PathUtil::shader_dir("general.frag"));
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT;

    ComputeShader* rayTraceComputeShader = new ComputeShader(PathUtil::shader_dir("ray_tracer.comp"));
    std::vector<SSBOBinding> rayTraceResources = _raytracerResources;
    DispatchCallback rayTraceDispatchCountCallback = [this](){ return glm::ivec3(mWindowSize.x, mWindowSize.y, 1); };
    UniformCallback rayTraceTextureCallback = [this](Shader* _shader){ BindTextures(_shader); };
    UniformCallback rayTraceCameraCallback = [this](Shader* _shader){ BindCamera(_shader); };
    UniformCallback rayTraceRefObjectsCallback = [this](Shader* _shader){ BindReferenceObjects(_shader); };
    UniformCallback rayTraceUniformsCallback = [this](Shader* _shader){ BindUniforms(_shader); };
    std::vector<UniformCallback> rayTraceUniforms = {rayTraceTextureCallback, rayTraceCameraCallback, rayTraceRefObjectsCallback, rayTraceUniformsCallback};
    mRaytracePass = new SimulationPass(rayTraceComputeShader, rayTraceResources, rayTraceDispatchCountCallback, rayTraceUniforms, passFlag);
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

void Renderer::BindCamera(const Shader* _shader)
{
    _shader->use();
    _shader->setVec3("camera.position", mCameraRef->GetPosition());
    _shader->setVec3("camera.up", mCameraRef->GetUp());
    _shader->setVec3("camera.right", mCameraRef->GetRight());
    _shader->setVec3("camera.front", mCameraRef->GetFront());
    _shader->setFloat("camera.fov", mCameraRef->GetFov());
    _shader->setFloat("camera.maxRayLength", mCameraRef->GetFarplane());
}

void Renderer::BindReferenceObjects(const Shader* _shader)
{
    _shader->use();
    for (Object* object : *mObjectsRef){
        BlackHole* blackHole = dynamic_cast<BlackHole*>(object);
        if(blackHole){
            _shader->setVec3("bh.position", blackHole->GetPosition());
            _shader->setFloat("bh.radius", blackHole->GetRadius());
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

void Renderer::BindUniforms(const class Shader* _shader)
{
    _shader->use();
    _shader->setFloat("cellSize", gCellSize);
    _shader->setFloat("particleRadius", gParticleRadius);
    _shader->setIVec3("gridSize", gGridSize);
    _shader->setVec3("gridMin", gGridBoundsMin);
    _shader->setVec3("gridMax", gGridBoundsMax);
    _shader->setInt("neighborRadius", gNeighborRadius);
}

void Renderer::SetWindowSize(glm::ivec2 _newWindowSize){
    
    mWindowSize = _newWindowSize;
    GenerateDisplayTexture();
}