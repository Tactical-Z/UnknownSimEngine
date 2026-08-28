#include "Engine/Simulation/SimulationPass.h"
#include "Shaders/ComputeShader.h"
#include "Util/Path.h"
#include "Util/Log.h"

SimulationPass::SimulationPass(ComputeShader* _shader, std::vector<SSBOBinding>& _resources, DispatchCallback _dispatchCountCallback, std::vector<UniformCallback>& _uniforms, ExecuteCallback _execute)
    :   mSolver(_shader), mResources(_resources), mDispatchCountCallback(_dispatchCountCallback), mUniforms(_uniforms), mExecuteCallback(_execute)
{
    mName = PathUtil::GetFilenameWithoutExtension(mSolver->GetSrc());
    GLint size[3];
    glGetProgramiv(mSolver->mId, GL_COMPUTE_WORK_GROUP_SIZE, size);
    mWorkGroupSize = glm::ivec3(size[0],size[1],size[2]);

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
    if(!mDispatchCountCallback)
        LOG_ERROR("SimulationPass: '{}' does not have valid dispatchCountCallback", mName);
    glm::ivec3 count = mDispatchCountCallback();
    glm::ivec3 groups = NumGroups(count);
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

glm::ivec3 SimulationPass::NumGroups(glm::ivec3 _count)
{
    glm::ivec3 groups = glm::ivec3(1);
    if(_count.x > 0)
        groups.x = (_count.x + mWorkGroupSize.x - 1) / mWorkGroupSize.x;
    if(_count.y > 0)
        groups.y = (_count.y + mWorkGroupSize.y - 1) / mWorkGroupSize.y;
    if(_count.z > 0)
        groups.z = (_count.z + mWorkGroupSize.z - 1) / mWorkGroupSize.z;
    return groups;
}

void SimulationPass::Dispatch(const glm::ivec3& _groupSize)
{
    glDispatchCompute(GLuint(_groupSize.x), GLuint(_groupSize.y), GLuint(_groupSize.z));
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

glm::ivec3 SimulationPass::GetWorkGroupSize()
{
    return mWorkGroupSize;
}