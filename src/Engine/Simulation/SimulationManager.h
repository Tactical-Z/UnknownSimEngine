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

private:
    // GPU buffers
    GPUBuffer<Particle> mParticleBuffer;
    GPUBuffer<HashEntry> mHashBuffer;
    GPUBuffer<uint32_t> mCellFlagBuffer;
    GPUBuffer<uint32_t> mCellPrefixBuffer;
    GPUBuffer<uint32_t> mCellStartBuffer;
    GPUBuffer<uint32_t> mCellEndBuffer;
    GPUBuffer<uint32_t> mCellCountBuffer;
    GPUBuffer<uint32_t> mUniqueHashBuffer;
    GPUBuffer<HashLookupEntry> mHashLookupBuffer;

    void InitBuffers();

    template<typename T>
    std::vector<T> ReadBuffer(GPUBuffer<T>& _buffer)
    {
        std::vector<T> data(_buffer.mTotalCount);

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            _buffer.mId
        );

        glGetBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            0,
            _buffer.GetSizeBytes(),
            data.data()
        );

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            0
        );

        return data;
    }

    // Simulation
    std::vector<class SimulationPipeline*> mPipelines;
    float mSimulationSpeed = 1.0f;

    // Simulation Data
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
    void BindSPHSmoothingRadius(class Shader* _shader);

    
    // ToDo: Update to use dynamic group allocation, currently only uses X invocation slot.
    void BindCustomExecuet_BitonicSort(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups);
    void BindCustomExecuet_BlellochScan_Up(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups);
    void BindCustomExecuet_BlellochScan_Down(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups);

public:

    class BlackHole* GetBlackHoleRefObject();
    float& GetSimulationSpeedRef();
    std::vector<SSBOBinding> GetRaytracerResources();
    std::vector<std::pair<const char*, float>> GetSimulationUIData();
};