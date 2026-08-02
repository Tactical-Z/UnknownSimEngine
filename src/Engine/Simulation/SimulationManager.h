#pragma once

#include "Engine/Simulation/SimData.h"
#include "Engine/Simulation/ParticleData.h"
#include <glad/glad.h>
#include <vector>
#include <array>
#include <functional>



class SimulationManager{
public:
    SimulationManager() = default;
    ~SimulationManager();

    void Init(std::vector<class Object*>& _referenceObjects);
    void Update(float _dt);
    void GenerateSimulation(SimulationType _simType);
    void BindSimulationStorageBuffers(int _layout);
    void BindBuffer(int _layout);

private:
    std::vector<class Simulator*> mSimulators;
    float mSimulationSpeed = 1.0f;

    bool mParticleIsGenerated = false;
    std::array<Particle, NUM_PARTICLE_MAX> mParticles;
    glm::vec2 mRadiusSpawnBounds = glm::vec2(1, 20); // Schwarzschild radius
    float mParticleRadius = 0.5f;
    std::vector<class Object*>* mReferenceObjectsRef;

    GLuint mParticleBufferID = 0;
    void GenerateSSBO(GLuint& _bufferId);
    template<typename T>
    void UploadSSBOData(T* _data, size_t _dataSize)
    {
        //glNamedBufferData(
        //    mParticleBufferID,
        //    _dataSize,
        //    _data,
        //    GL_DYNAMIC_DRAW
        //);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleBufferID);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            _dataSize,
            _data,
            GL_DYNAMIC_DRAW
        );
    
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void BindCallbacksToFunctions();
    using DTCallback = std::function<void(class Shader*)>;
    DTCallback mDTCallback;
    void BindDTCallback(class Shader* _shader);
    using CCallback = std::function<void(class Shader*)>;
    CCallback mCCallback;
    void BindCCallback(class Shader* _shader);
    using GCallback = std::function<void(class Shader*)>;
    GCallback mGCallback;
    void BindGCallback(class Shader* _shader);
    using SCallback = std::function<void(class Shader*)>;
    SCallback mSCallback;
    void BindSCallback(class Shader* _shader);
    using RefObjCallback = std::function<void(class Shader*)>;
    RefObjCallback mRefObjCallback;
    void BindRefObjCallback(class Shader* _shader);

    void InitParticlesAccretionDisk(const class BlackHole* _referenceObject);
    class BlackHole* GetBlackHoleRefObject();

public:
    float& GetSimulationSpeedRef();
};