#pragma once

#include "Engine/Simulation/Simulator.h"
#include "Engine/Simulation/ParticleData.h"
#include "Util/SMath.h"
#include <array>
#include <string>

class AccretionSimulator : public Simulator
{
public:
    AccretionSimulator(SimulationType _simType, std::string _solverSrcPath, const class Object* _referenceObject);
    ~AccretionSimulator();

    void TestPrintParticleData() override;
    void PerformSimulation() override;
private:
    std::array<Particle, NUM_PARTICLE_MAX> mParticles;
    glm::vec2 mRadiusSpawnBounds = glm::vec2(1, 40); // Schwarzschild radius
    float mParticleRadius = 0.5f;
    
    void InitAccretionDiskSimulation();
    void InitParticles(const class BlackHole* _referenceObject);
public:

};