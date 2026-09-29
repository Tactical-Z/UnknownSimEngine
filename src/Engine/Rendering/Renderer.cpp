#include "engine/rendering/Renderer.h"
#include "engine/Camera.h"
#include "engine/simulation/SimData.h"
#include "engine/simulation/SimulationPass.h"
#include "shaders/ComputeShader.h"
#include "shaders/VisShader.h"
#include "engine/rendering/Texture.h"
#include "engine/Camera.h"
#include "engine/Objects.h"

#include "core/clock/Clock.h"
#include "core/errorHandling/Log.h"
#include "util/AppUtil.h"

#include <glad/glad.h>

void Renderer::Init(Logger* _logger, Clock* _clock, std::vector<SSBOBinding> _raytracerResources, const Camera* _camera, const std::vector<class Object*>* _objects)
{
    mLogger = _logger;
    mClock = _clock;
    mCameraRef = _camera;
    mObjectsRef = _objects;

    InitBuffers();
    InitShaders(_raytracerResources);
    InitTextures();
}

void Renderer::StartFrame()
{

}

void Renderer::Update(const float& _dt)
{

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


void Renderer::EndFrame()
{

}

void Renderer::Shutdown()
{
    delete mDisplayTexture;
    delete mVisShader;
    delete mSkyboxTexture;
    delete mRaytracePass;

    // Not owned by this class
    mCameraRef = nullptr;
    mObjectsRef = nullptr;
}

void Renderer::InitBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
}

void Renderer::InitShaders(std::vector<SSBOBinding> _raytracerResources)
{
    mVisShader = new VisShader(AppUtil::Path::shader_dir("general.vert"), AppUtil::Path::shader_dir("general.frag"), mLogger);
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT;

    ComputeShader* rayTraceComputeShader = new ComputeShader(AppUtil::Path::shader_dir("ray_tracer.comp"), mLogger);
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
    mSkyboxTexture = new Texture(AppUtil::Path::skybox_files("skybox/blue/"));
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
            _shader->setFloat("bh.mass", blackHole->GetMass());
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
    _shader->setFloat("C", Math::Constants::LightSpeed);
    _shader->setFloat("G", Math::Constants::GravitationalConstant);
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