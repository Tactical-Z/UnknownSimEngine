#include "Engine/Simulation/Simulator.h"
#include "Shaders/ComputeShader.h"
#include "Util/Log.h"

Simulator::Simulator(SimulationType _simType, std::string _solverSrcPath)
    : mSimulationType(_simType)
{
    GenerateSSBO(mParticleBufferID);
    GenerateSimulationSolver(_solverSrcPath);
}

Simulator::~Simulator()
{
    delete mSimulationSolver;
    mSimulationSolver = nullptr;

    glDeleteBuffers(1, &mParticleBufferID);
    mParticleBufferID = 0;
}

void Simulator::GenerateSimulationSolver(std::string _solverSrcPath)
{
    mSimulationSolver = new ComputeShader(_solverSrcPath);
}

void Simulator::BindBuffer(int _layout)
{
    // ToDo: 1 here is the "location" in compute shder, should make a better system 
    // for calling the correct location based on type or something like that.
    // layout 1 since texture is taking up layout 0
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _layout, mParticleBufferID);
}

void Simulator::GenerateSSBO(GLuint& _bufferId)
{
    glGenBuffers(1, &_bufferId);
}