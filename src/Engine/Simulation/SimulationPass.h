#pragma once

#include "Engine/Simulation/SimData.h"
#include <string>
#include <vector>
#include <glad/glad.h>

class SimulationPass
{
public:
    SimulationPass() = default;
    SimulationPass(class ComputeShader* _shader, 
                    std::vector<SSBOBinding> _resources, 
                    DispatchCallback _dispatchCountCallback, 
                    std::vector<UniformCallback> _uniforms,
                    ExecuteCallback _execute = nullptr);
    ~SimulationPass();
    void Execute();
    void Dispatch(const glm::ivec3& _groupSize);

private: 
    std::string mName;
    class ComputeShader* mSolver = nullptr;
    DispatchCallback mDispatchCountCallback;
    glm::ivec3 mWorkGroupSize;
    std::vector<SSBOBinding> mResources;
    std::vector<UniformCallback> mUniforms;
    ExecuteCallback mExecuteCallback;
    bool mIsEnabled = true;
    GLuint mTimeQuery = 0;
    float mGPUTimeMS = 0;

    void UpdateTimer();
    void BindResources();
    void BindUniforms();
public:
    glm::ivec3 NumGroups(glm::ivec3 _count);
    bool IsEnabled();
    void SetIsEnabled(bool _b);
    std::string& GetName();
    float GetGPUTimeMS() const;
    class ComputeShader* GetShader();
    glm::ivec3 GetWorkGroupSize();
};