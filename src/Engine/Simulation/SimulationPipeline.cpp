#include "Engine/Simulation/SimulationPipeline.h"
#include "Engine/Simulation/SimulationPass.h"

SimulationPipeline::~SimulationPipeline()
{
    for(SimulationPass* pass : mSimulationPasses){
        delete pass;
        pass = nullptr;
    }
    mSimulationPasses.clear();
}   

void SimulationPipeline::ExecutePipeline(const uint32_t& _count)
{
    
    for(SimulationPass* pass : mSimulationPasses){
        if(pass->IsEnabled())
        pass->Execute(_count);
    }
}

void SimulationPipeline::AddPass(SimulationPass&& _pass)
{
    mSimulationPasses.push_back(std::move(_pass));
}

SimulationPass* SimulationPipeline::GetPass(const char* _s)
{
    
}

void SimulationPipeline::DisablePass(const char* _s)
{
    
}

void SimulationPipeline::EnablePass(const char* _s)
{
    
}
