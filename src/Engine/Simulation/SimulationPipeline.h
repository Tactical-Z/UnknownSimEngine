#pragma once

#include <string>
#include <vector>
#include <utility>

class SimulationPipeline{
public:
    SimulationPipeline() = default;
    SimulationPipeline(std::string _name);
    ~SimulationPipeline();

    void ExecutePipeline();

private:
    std::vector<class SimulationPass*> mSimulationPasses;
    std::string mName;
public:
    const char* GetName();
    void AddPass(class SimulationPass* _pass);
    const SimulationPass* GetPass(const char* _s);
    void EnablePass(const char* _s, bool _b);
    void GetUIData(std::vector<std::pair<const char*, float>>& _uiData);
};