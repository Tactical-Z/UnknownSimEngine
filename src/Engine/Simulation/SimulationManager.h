#pragma once

#include "Engine/Simulation/SimData.h"
#include <utility>

class SimulationManager{
public:
    SimulationManager() = default;
    ~SimulationManager();

    void Init(const std::vector<class Object*>& _referenceObjects);
    void Update(float _dt);

    void BindBuffer(GLint _bufferID, int _layout);

private:
    // --- GPU buffers ---
    GPUBuffer<Particle> mParticleBuffer;
    
    // SPH Buffers
    GPUBuffer<HashEntry> mHashBuffer;
    GPUBuffer<uint32_t> mCellFlagBuffer;
    GPUBuffer<uint32_t> mCellPrefixBuffer;
    GPUBuffer<uint32_t> mCellCountBuffer;
    GPUBuffer<uint32_t> mCellStartBuffer;
    GPUBuffer<uint32_t> mCellEndBuffer;
    GPUBuffer<uint32_t> mUniqueHashBuffer;
    GPUBuffer<HashEntry> mHashLookupBuffer;

    // Render Buffers
    GPUBuffer<HashEntry> mRenderGridBuffer;
    GPUBuffer<uint32_t> mRenderCellFlagBuffer;
    GPUBuffer<uint32_t> mRenderCellPrefixBuffer;
    GPUBuffer<uint32_t> mRenderCellCountBuffer;
    GPUBuffer<uint32_t> mRenderCellStartBuffer;
    GPUBuffer<uint32_t> mRenderCellEndBuffer;
    

    void InitBuffers();
  
    // Debug--
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
    void ValidateRenderGrid(); // Do not remove, forever imortalized as the Hail mary function responsible for solving the fucked range bug for dense render grid.
    bool ValidateHashGrid();

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
    void CreateDGPipeline();

    // Helper pipeline creation functions
    void CreateBitonicSortPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer);
    void CreateFlagPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer, GPUBuffer<uint32_t>& _flagBuffer, GPUBuffer<uint32_t>& _prefixBuffer);
    void CreateBlellochScanPass(SimulationPipeline* _pipeline, GPUBuffer<uint32_t>& _prefixBuffer, GPUBuffer<uint32_t>& _countBuffer);

    // Binding functions for callback
    void BindCallback(class Shader* _shader, const char* _varName, unsigned int _val);
    void BindCallback(class Shader* _shader, const char* _varName, int _val);
    void BindCallback(class Shader* _shader, const char* _varName, float _val);
    void BindCallback(class Shader* _shader, const char* _varName, glm::ivec3 _val);
    void BindCallback(class Shader* _shader, const char* _varName, glm::vec3 _val);
    void BindDTCallback(class Shader* _shader);
    void BindCCallback(class Shader* _shader);
    void BindGCallback(class Shader* _shader);
    void BindSCallback(class Shader* _shader);
    void BindActiveCountCallback(class Shader* _shader);
    void BindRefObjCallback(class Shader* _shader);  
    void BindCellSizeCallback(class Shader* _shader);
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