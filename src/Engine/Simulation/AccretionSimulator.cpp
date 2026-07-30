#include "Engine/Simulation/AccretionSimulator.h"
#include "Shaders/ComputeShader.h"
#include "Engine/Objects.h"
#include "Util/Log.h"

AccretionSimulator::AccretionSimulator(SimulationType _simType, std::string _solverSrcPath, const Object* _referenceObject)
    :   Simulator(_simType, _solverSrcPath)
{
    const BlackHole* blackHoleReferenceObject = static_cast<const BlackHole*>(_referenceObject);
    InitParticles(blackHoleReferenceObject);

    UploadSSBOData(mParticles.data(), NUM_PARTICLE_MAX * sizeof(Particle));
}

AccretionSimulator::~AccretionSimulator()
{

}

void AccretionSimulator::TestPrintParticleData()
{
    for(Particle& particle : mParticles)
    {
        LOG_DEBUG("particle x: {}, y: {}, z: {}", particle.mPosition.x, particle.mPosition.y, particle.mPosition.z);
    }
}

void AccretionSimulator::PerformSimulation()
{
    mSimulationSolver->use();
    BindBuffer(0);
    glDispatchCompute(mParticles.size(), 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void AccretionSimulator::InitParticles(const class BlackHole* _referenceObject)
{
    if(_referenceObject){
        // spawn particles in relation to refernce object.
        for(Particle& particle : mParticles)
        {
            float randomAngle = SMath::RandomFloatByBounds(0, TWO_PI);
            float randomRadius = SMath::RandomFloatByBounds(_referenceObject->GetRadius() + mRadiusSpawnBounds);
            float randomZDeviation = SMath::RandomFloatByBounds(-0.05f, 0.05f);
            particle.mPosition = glm::vec4( _referenceObject->GetPosition().x + (randomRadius * cos(randomAngle)),
                                            _referenceObject->GetPosition().y + (randomRadius * sin(randomAngle)),
                                            _referenceObject->GetPosition().z + (randomZDeviation),
                                            mParticleRadius); 
        }
    } else {
        // Spawn particles randomly near centre of world
        for(Particle& particle : mParticles)
        {
            glm::vec3 min = glm::vec3(0.5f, -0.5f, -0.1f);
            glm::vec3 max = glm::vec3(1.5f, 0.5f, 0.1f);
            glm::vec3 newPosition = SMath::RandomVecByBounds(min,max);
            particle.mPosition = glm::vec4(newPosition, 0.1f);
        }
    }


}


