#pragma once

#include <vector>

class SimulationPipeline{
public:
    SimulationPipeline() = default;
    ~SimulationPipeline();

    void ExecutePipeline(const uint32_t& _count);

private:
    std::vector<class SimulationPass*> mSimulationPasses;

public:
    void AddPass(SimulationPass&& _pass);
    SimulationPass* GetPass(const char* _s);
    void DisablePass(const char* _s);
    void EnablePass(const char* _s);
};