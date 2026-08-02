#pragma once

#include "Engine/Simulation/SimData.h"
#include "Engine/Simulation/ParticleData.h"
#include "glad/glad.h"
#include <array>
#include <string>
#include <vector>
#include <functional>

class Simulator
{
public:
    Simulator() = default;
    Simulator(SimulationType _simType, std::string _solverSrcPath, std::vector<std::function<void(class Shader*)>> _bindFunctions);
    ~Simulator();

    void PerformSimulation(GLuint _groupSize);

private:
    SimulationType mSimulationType;
    class ComputeShader* mSimulationSolver = nullptr;
    std::vector<std::function<void(class Shader*)>> mBindFunctions;
    
    void GenerateSimulationSolver(std::string _solverSrcPath);
    void BindUniforms();

public:
    SimulationType GetType();
};