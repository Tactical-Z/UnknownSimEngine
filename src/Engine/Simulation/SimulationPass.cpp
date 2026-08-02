#include "Engine/Simulation/SimulationPass.h"
#include "Shaders/ComputeShader.h"
#include "Util/Path.h"

SimulationPass::SimulationPass(ComputeShader* _shader, uint32_t _dispachSize, std::vector<SSBOBinding> _resources, std::vector<UniformCallback> _uniforms)
    :   mSovler(_shader), mDispachSize(_dispachSize), mResources(_resources), mUniforms(_uniforms)
{
    mName = mSolver->GetSrc();
}

SimulationPass::~SimulationPass()
{
    delete mSovler;
    mSovler = nullptr;
}

void SimulationPass::Execute(const uint32_t& _count)
{
    if(!IsEnabled) return;

    GLuint groups = ceil(_count / mDispachSize);
    mSovler->use();
    BindResources();
    BindUniforms();
    Dispach(groups);
}

void SimulationPass::BindResources()
{
    for(SSBOBinding& resource : mResources){
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER,
            (GLint)resource.mLocation,
            resource.mBufferID
        );
    }
}

void SimulationPass::BindUniforms()
{
    for(UniformCallback uniform : mUniforms){
        if(uniform)
            uniform(mSovler);
    }
}

void SimulationPass::Dispach(const GLuint& _groupSize)
{
    glDispatchCompute(_groupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

bool SimulationPass::IsEnabled()
{
    return mIsEnabled;
}

bool SimulationPass::SetIsEnabled(bool _b)
{
    mIsEnabled = _b;
}