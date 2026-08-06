#include "Engine/Simulation/SimulationPass.h"
#include "Shaders/ComputeShader.h"
#include "Util/Path.h"
#include "Util/Log.h"

SimulationPass::SimulationPass(ComputeShader* _shader, std::vector<SSBOBinding> _resources, DispatchCallback _dispatchCountCallback, std::vector<UniformCallback> _uniforms, ExecuteCallback _execute)
    :   mSolver(_shader), mResources(_resources), mDispatchCountCallback(_dispatchCountCallback), mUniforms(_uniforms), mExecuteCallback(_execute)
{
    mName = PathUtil::GetFilenameWithoutExtension(mSolver->GetSrc());
    GLint size[3];
    glGetProgramiv(mSolver->mId, GL_COMPUTE_WORK_GROUP_SIZE, size);
    mWorkGroupSize = size[0];

    glGenQueries(1, &mTimeQuery);
}

SimulationPass::~SimulationPass()
{
    glDeleteQueries(1, &mTimeQuery);
    delete mSolver;
    mSolver = nullptr;
}

void SimulationPass::Execute()
{
    if(!mIsEnabled) return;
    UpdateTimer();
    
    glBeginQuery(GL_TIME_ELAPSED, mTimeQuery);
    uint32_t count = mDispatchCountCallback();
    uint32_t groups = (count + mWorkGroupSize - 1) / mWorkGroupSize;
    mSolver->use();
    BindResources();
    BindUniforms();
    if(mExecuteCallback){
        mExecuteCallback(this, count, groups);
    } else {
        Dispatch(groups);
    }   
    glEndQuery(GL_TIME_ELAPSED);
}

void SimulationPass::UpdateTimer()
{
    GLuint available = 0;

    glGetQueryObjectuiv(mTimeQuery, GL_QUERY_RESULT_AVAILABLE, &available);

    if(available)
    {
        GLuint64 time;

        glGetQueryObjectui64v(mTimeQuery, GL_QUERY_RESULT, &time);

        mGPUTimeMS = time / 1000000.0f;
    }
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
            uniform(mSolver);
    }
}

void SimulationPass::Dispatch(const GLuint& _groupSize)
{
    glDispatchCompute(_groupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

bool SimulationPass::IsEnabled()
{
    return mIsEnabled;
}

void SimulationPass::SetIsEnabled(bool _b)
{
    mIsEnabled = _b;
}

std::string& SimulationPass::GetName(){
    return mName;
}

float SimulationPass::GetGPUTimeMS() const
{
    return mGPUTimeMS;
} 

ComputeShader* SimulationPass::GetShader()
{
    return mSolver;
}

uint32_t SimulationPass::GetWorkGroupSize()
{
    return mWorkGroupSize;
}