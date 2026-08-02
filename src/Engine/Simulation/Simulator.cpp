#include "Engine/Simulation/Simulator.h"
#include "Shaders/ComputeShader.h"
#include "Util/Log.h"

Simulator::Simulator(SimulationType _simType, std::string _solverSrcPath, std::vector<std::function<void(Shader*)>> _bindFunctions)
    :   mSimulationType(_simType), mBindFunctions(_bindFunctions)
{
    GenerateSimulationSolver(_solverSrcPath);
}

Simulator::~Simulator()
{
    delete mSimulationSolver;
    mSimulationSolver = nullptr;
}

void Simulator::GenerateSimulationSolver(std::string _solverSrcPath)
{
    mSimulationSolver = new ComputeShader(_solverSrcPath);
}

void Simulator::PerformSimulation(GLuint _groupSize)
{
    mSimulationSolver->use();
    BindUniforms();
    
    glDispatchCompute(_groupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Simulator::BindUniforms()
{
    for(std::function<void(Shader*)> bindFunction : mBindFunctions)
    {
        if(bindFunction)
            bindFunction(mSimulationSolver);
    }
}

SimulationType Simulator::GetType()
{
    return mSimulationType;
}