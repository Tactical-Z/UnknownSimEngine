#include "Engine/Simulation/SimulationManager.h"
#include "Engine/Simulation/Simulator.h"
#include "Engine/Objects.h"
#include "Shaders/Shader.h"
#include "Util/SMath.h"
#include "Util/Path.h"
#include "Util/Log.h"
#include "Util/STime.h"

SimulationManager::~SimulationManager()
{
    for(Simulator* simulator : mSimulators){
        delete simulator;
        simulator = nullptr;
    }
    mSimulators.clear();

    glDeleteBuffers(1, &mParticleBufferID);
    mParticleBufferID = 0;
}

void SimulationManager::Init(std::vector<class Object*>& _referenceObjects)
{
    mReferenceObjectsRef = &_referenceObjects;
    BindCallbacksToFunctions();
    GenerateSSBO(mParticleBufferID);
    
    for(Object* obj : *mReferenceObjectsRef){
        if(obj){
            if(BlackHole* blackHole = static_cast<BlackHole*>(obj)){
                InitParticlesAccretionDisk(blackHole);
                UploadSSBOData(mParticles.data(), NUM_PARTICLE_MAX * sizeof(Particle));
                break;
            }
        }
    }
}

void SimulationManager::Update(float _dt)
{
    BindBuffer(0);
    GLuint groups = (NUM_PARTICLE_MAX + 9) / 10;
    for(Simulator* simulator : mSimulators){
        simulator->PerformSimulation(groups);
    }
}

void SimulationManager::GenerateSimulation(SimulationType _simType)
{
    switch (_simType)
    {
    case SimulationType::ST_ACCRETIONDISK_SPH:
        if(mParticleIsGenerated){
            std::vector<std::function<void(Shader*)>> bindFunctions = {};
            mSimulators.push_back(new Simulator(_simType, PathUtil::shader_dir("accretionDiskSPHSolver.comp"), bindFunctions));
        }
        break;
    case SimulationType::ST_ACCRETIONDISK_GRAV:
        if(mParticleIsGenerated){
            std::vector<std::function<void(Shader*)>> bindFunctions = {mDTCallback, mGCallback, mRefObjCallback};
            mSimulators.push_back(new Simulator(_simType, PathUtil::shader_dir("accretionDiskGravSolver.comp"), bindFunctions));
        }
        break;
    case SimulationType::ST_SPACIAL_HASH_GRID:
        if(mParticleIsGenerated){
            std::vector<std::function<void(Shader*)>> bindFunctions = {};
            mSimulators.push_back(new Simulator(_simType, PathUtil::shader_dir("hashGridGenerator.comp"), bindFunctions));
        }
        break;
    default:
        LOG_WARNING("Unkown Simulation Type, simulator not initiated.");
        break;
    }
}

void SimulationManager::GenerateSSBO(GLuint& _bufferId)
{
    glGenBuffers(1, &_bufferId);
}

void SimulationManager::BindBuffer(int _layout)
{
    // ToDo: 1 here is the "location" in compute shder, should make a better system 
    // for calling the correct location based on type or something like that.
    // layout 1 since texture is taking up layout 0
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _layout, mParticleBufferID);
}

void SimulationManager::BindCallbacksToFunctions()
{
    mDTCallback = [this](Shader* _shader){ BindDTCallback(_shader); };
    mCCallback = [this](Shader* _shader){ BindCCallback(_shader); };
    mGCallback = [this](Shader* _shader){ BindGCallback(_shader); };
    mSCallback = [this](Shader* _shader){ BindSCallback(_shader); };
    mRefObjCallback = [this](Shader* _shader){ BindRefObjCallback(_shader); };
}

void SimulationManager::BindDTCallback(Shader* _shader)
{
    _shader->use();
    float dt = STime::GetDeltaTime() * mSimulationSpeed;
    _shader->setFloat("dt", dt);
}

void SimulationManager::BindCCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setInt("C", C);
}

void SimulationManager::BindGCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setInt("G", G);
}

void SimulationManager::BindSCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setInt("S", S);
}

void SimulationManager::BindRefObjCallback(class Shader* _shader)
{
    BlackHole* bh = GetBlackHoleRefObject();
    if(bh){
        _shader->use();
        _shader->setVec3("refObj.position", bh->GetPosition());
        _shader->setFloat("refObj.radius", bh->GetRadius());
        _shader->setFloat("refObj.mass", bh->GetMass());
    }
}

void SimulationManager::InitParticlesAccretionDisk(const class BlackHole* _referenceObject)
{
    mParticleIsGenerated = true;
    if(_referenceObject){
        
        // spawn particles in relation to refernce object.
        for(Particle& particle : mParticles)
        {
            float randomAngle = SMath::RandomFloatByBounds(0, TWO_PI);
            float randomRadius = SMath::RandomFloatByBounds(_referenceObject->GetRadius() + mRadiusSpawnBounds);
            float randomZDeviation = SMath::RandomFloatByBounds(-0.05f, 0.05f);
            particle.mPosition = glm::vec3( _referenceObject->GetPosition().x + (randomRadius * cos(randomAngle)),
                                            _referenceObject->GetPosition().y + (randomRadius * sin(randomAngle)),
                                            _referenceObject->GetPosition().z + (randomZDeviation));

            particle.mRadius = mParticleRadius;
            
            // finding tangental vector by differentiating inital position rotation. x: cos -> -sin, y: sin -> cos
            float initalOrbitalVelocity = SMath::OrbitalVelocity(randomRadius, _referenceObject->GetMass());
            particle.mVelocity = glm::vec3(-sin(randomAngle) * initalOrbitalVelocity,
                                            cos(randomAngle) * initalOrbitalVelocity,
                                            0);

            particle.mMass = 0.f;
        }
    } else {
        // Spawn particles randomly near centre of world
        for(Particle& particle : mParticles)
        {
            //glm::vec3 min = glm::vec3(0.5f, -0.5f, -0.1f);
            //glm::vec3 max = glm::vec3(1.5f, 0.5f, 0.1f);
            //glm::vec3 newPosition = SMath::RandomVecByBounds(min,max);
            //particle.mPosition = glm::vec4(newPosition, 0.1f);
        }
    }
}

BlackHole* SimulationManager::GetBlackHoleRefObject()
{
    int counter = 0;
    BlackHole* bh = nullptr;
    for(Object* obj : *mReferenceObjectsRef){
        if(obj){
            if(BlackHole* blackHole = static_cast<BlackHole*>(obj)){
                bh = blackHole;
                counter++;
            }
        }
    }
    if(counter > 1)
        LOG_WARNING("Multiple black holes detected while getting refernec object, can lead to undefined behaviour");
    return bh;
}

float& SimulationManager::GetSimulationSpeedRef()
{
    return mSimulationSpeed;
}