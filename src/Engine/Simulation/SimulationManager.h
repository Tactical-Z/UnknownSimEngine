#pragma once

#include "Engine/Simulation/SimData.h"
#include "Engine/Simulation/ParticleData.h"
#include "Engine/Simulation/HashGridData.h"
#include <utility>

class SimulationManager{
public:
    SimulationManager() = default;
    ~SimulationManager();

    void Init(const std::vector<class Object*>& _referenceObjects);
    void Update(float _dt);

    void BindBuffer(GLint _bufferID, int _layout);
    void BindParticleBuffer();
private:
    // GPU buffers
    GPUBuffer<Particle> mParticleBuffer;
    GPUBuffer<HashEntry> mHashBuffer;
    GPUBuffer<uint32_t> mCellStartBuffer;
    GPUBuffer<uint32_t> mCellEndBuffer;
    //GLuint mParticleBuffer;
    //GLuint mHashBuffer;
    //GLuint mCellStartBuffer;
    //GLuint mCellEndBuffer;

    void InitBuffers();

    // Simulation
    std::vector<class SimulationPipeline*> mPipelines;
    float mSimulationSpeed = 1.0f;

    // Simulation Data
    //std::array<Particle, NUM_PARTICLE_MAX> mParticles;
    float mParticleRadius = 0.5f;
    glm::vec2 mRadiusSpawnBounds = glm::vec2(1, 20); // Schwarzschild radius
    
    void GenerateAccretionDiskParticles();

    // Reference data
    const std::vector<class Object*>* mReferenceObjectsRef;

    // ---------------------------------------------------

    // Pipeline Creation
    void CreateGravityPipeline();
    void CreateSHGPipeline();
    void CreateSPHPipeline();

    // Binding functions for callback
    void BindDTCallback(class Shader* _shader);
    void BindCCallback(class Shader* _shader);
    void BindGCallback(class Shader* _shader);
    void BindSCallback(class Shader* _shader);
    void BindActiveCountCallback(class Shader* _shader);
    void BindRefObjCallback(class Shader* _shader);  
    void BindSHGCellSizeCallback(class Shader* _shader);

    void BindCustomExecuet_BitonicSort(class SimulationPass* _pass, uint32_t _count, uint32_t _groups);

public:

    class BlackHole* GetBlackHoleRefObject();
    float& GetSimulationSpeedRef();
    std::vector<std::pair<const char*, float>> GetSimulationUIData();
};