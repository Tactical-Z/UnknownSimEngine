#pragma once

#include "Engine/Simulation/SimData.h"
#include <vector>
#include <glad/glad.h>

class SimulationPass
{
public:
    SimulationPass() = default;
    SimulationPass(class ComputeShader* _shader, uint32_t _dispachSize, std::vector<SSBOBinding> _resources, std::vector<UniformCallback> _uniforms);
    ~SimulationPass();

    void Execute(const GLuint& _groupSize);

private: 
    std::string mName;
    class ComputeShader* mSovler = nullptr;
    uint32_t mDispachSize = 0;
    std::vector<SSBOBinding> mResources;
    std::vector<UniformCallback> mUniforms;
    bool mIsEnabled = true;

    void BindResources();
    void BindUniforms();
    void Dispach(const uint32_t& _count);
public:
    bool IsEnabled();
    bool SetIsEnabled(bool _b);
};