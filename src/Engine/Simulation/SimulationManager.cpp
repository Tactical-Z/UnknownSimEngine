#include "Engine/Simulation/SimulationManager.h"
#include "Engine/Simulation/Simulator.h"
#include "Engine/Simulation/AccretionSimulator.h"
#include "Engine/Objects.h"
#include "Util/Path.h"
#include "Util/Log.h"

SimulationManager::~SimulationManager()
{
    for(Simulator* simulator : mSimulators){
        delete simulator;
        simulator = nullptr;
    }
    mSimulators.clear();
}

void SimulationManager::Init()
{

}

void SimulationManager::Update(float _dt)
{
    for(Simulator* simulator : mSimulators){
        simulator->PerformSimulation();
    }
}

void SimulationManager::GenerateSimulation(SimulationType _simType, const Object* _referenceObject)
{
    
    switch (_simType)
    {
    case SimulationType::ST_ACCRETIONDISK:
        mSimulators.push_back(new AccretionSimulator(_simType, PathUtil::shader_dir("accretionDiskSovler.comp"), _referenceObject));
        break;
    
    default:
        LOG_WARNING("Unkown Simulation Type, simulator not initiated.");
        break;
    }
}

void SimulationManager::BindSimulationStorageBuffers(int _layout)
{
    for(Simulator* simulator : mSimulators){
        simulator->BindBuffer(_layout);
        //simulator->TestPrintParticleData();
    }
}