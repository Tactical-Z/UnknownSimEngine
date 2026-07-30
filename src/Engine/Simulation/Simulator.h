#pragma once

#include "Engine/Simulation/SimData.h"
#include "glad/glad.h"
#include <array>
#include <string>

class Simulator
{
public:
    Simulator() = default;
    Simulator(SimulationType _simType, std::string _solverSrcPath);
    virtual ~Simulator();

    void BindBuffer(int _layout); // Binds this shader to the layout required to inmport particle data.
    virtual void PerformSimulation() = 0;
    virtual void TestPrintParticleData() = 0;
    

private:
    GLuint mParticleBufferID = 0;

    void GenerateSimulationSolver(std::string _solverSrcPath);

protected:
    class ComputeShader* mSimulationSolver = nullptr;
    SimulationType mSimulationType;

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

public:

};