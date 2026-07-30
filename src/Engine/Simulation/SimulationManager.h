#pragma once

#include "Engine/Simulation/SimData.h"
#include <vector>

class SimulationManager{
public:
    SimulationManager() = default;
    ~SimulationManager();

    void Init();
    void Update(float _dt);
    void GenerateSimulation(SimulationType _simType, const class Object* _referenceObject);
    void BindSimulationStorageBuffers(int _layout);

private:
    std::vector<class Simulator*> mSimulators;



public:

};