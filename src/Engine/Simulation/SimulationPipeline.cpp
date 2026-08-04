#include "Engine/Simulation/SimulationPipeline.h"
#include "Engine/Simulation/SimulationPass.h"

SimulationPipeline::SimulationPipeline(std::string _name)
    :   mName(_name)
{}   

SimulationPipeline::~SimulationPipeline()
{
    for(SimulationPass* pass : mSimulationPasses){
        delete pass;
        pass = nullptr;
    }
    mSimulationPasses.clear();
}   

void SimulationPipeline::ExecutePipeline()
{
    for(SimulationPass* pass : mSimulationPasses){
        if(pass->IsEnabled())
            pass->Execute();
    }
}

const char* SimulationPipeline::GetName()
{
    return mName.c_str();
}

void SimulationPipeline::AddPass(SimulationPass* _pass)
{
    mSimulationPasses.push_back(std::move(_pass));
}

const SimulationPass* SimulationPipeline::GetPass(const char* _s)
{
    for(SimulationPass* pass : mSimulationPasses){
        if(pass->GetName() == _s)
            return pass;
    }
}

void SimulationPipeline::EnablePass(const char* _s, bool _b)
{
    for(SimulationPass* pass : mSimulationPasses){
        if(pass->GetName() == _s)
            pass->SetIsEnabled(_b);
    }
}   

void SimulationPipeline::GetUIData(std::vector<std::pair<const char*, float>>& _uiData){

    _uiData.push_back(std::make_pair(GetName(), -1));
    for(SimulationPass* pass : mSimulationPasses){
        _uiData.push_back(std::make_pair(pass->GetName().c_str(),pass->GetGPUTimeMS()));
    }
}