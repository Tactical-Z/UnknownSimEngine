#include "Engine/Simulation/SimulationManager.h"
#include "Engine/Simulation/SimulationPipeline.h"
#include "Engine/Simulation/SimulationPass.h"
#include "Engine/Objects.h"
#include "Shaders/ComputeShader.h"
#include "Util/Log.h"
#include "Util/Path.h"
#include "Util/STime.h"

#include <bit>

SimulationManager::~SimulationManager()
{
    for(SimulationPipeline* pipelines : mPipelines){
        delete pipelines;
        pipelines = nullptr;
    }
    mPipelines.clear();

    glDeleteBuffers(1, &mParticleBuffer.mId);
    mParticleBuffer.mId = 0;
    glDeleteBuffers(1, &mHashBuffer.mId);
    mHashBuffer.mId = 0;
    glDeleteBuffers(1, &mCellStartBuffer.mId);
    mCellStartBuffer.mId = 0;
    glDeleteBuffers(1, &mCellEndBuffer.mId);
    mCellEndBuffer.mId = 0;
}

void SimulationManager::Init(const std::vector<class Object*>& _referenceObjects)
{
    mReferenceObjectsRef = &_referenceObjects;
    
    InitBuffers();

    CreateGravityPipeline();
    CreateSHGPipeline();
    CreateSPHPipeline();
}

void SimulationManager::Update(float _dt)
{
    for(SimulationPipeline* pipline : mPipelines){
        pipline->ExecutePipeline();
    }

    // LOG_WARNING("Hashes: ");
    // auto hashes = ReadBuffer(mHashBuffer);
    // for(uint32_t i = 0; i < hashes.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, Hash: {}, particleIndex: {}", i, hashes[i].hash,hashes[i].particleIndex);
    // }

    // LOG_WARNING("Flag buffer: ");
    // auto flag = ReadBuffer(mCellFlagBuffer);
    // for(uint32_t i = 0; i < flag.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, flag: {}", i, flag[i]);
    // }

    // LOG_WARNING("Prefix buffer: ");
    // auto prefix = ReadBuffer(mCellPrefixBuffer);
    // for(uint32_t i = 0; i < prefix.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, prefix: {}", i, prefix[i]);
    // }

    // LOG_WARNING("CellStart: ");
    // auto cellStart = ReadBuffer(mCellStartBuffer);
    // for(uint32_t i = 0; i < cellStart.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, cellStart: {}", i, cellStart[i]);
    // }

    // LOG_WARNING("Cell End: ");
    // auto cellEnd = ReadBuffer(mCellEndBuffer);
    // for(uint32_t i = 0; i < cellEnd.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, cellEnd: {}", i, cellEnd[i]);
    // }

    // LOG_WARNING("Unique Hashes: ");
    // auto uniqeHash = ReadBuffer(mUniqueHashBuffer);
    // for(uint32_t i = 0; i < uniqeHash.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, uniqe hash: {}", i, uniqeHash[i]);
    // }
    
    // auto count = ReadBuffer(mCellCountBuffer);
    // LOG_WARNING("Num cells: {}", count[0]);
    
    LOG_WARNING("Lookuptable: ");
    auto table = ReadBuffer(mHashLookupBuffer);
    for(uint32_t i = 0; i < table.size(); i++)
    {
        LOG_DEBUG("Index: {}, Hash: {}, cell: {}", i, table[i].hash, table[i].cellIndex);
    }
    LOG_DEBUG("End");
}

void SimulationManager::BindBuffer(GLint _bufferID, int _layout)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _layout, _bufferID);
}

void SimulationManager::BindParticleBuffer()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLint)BindingLocation::BL_PARTICLE_BUFFER, mParticleBuffer.mId);
}

void SimulationManager::InitBuffers()
{
    mParticleBuffer.mBindingLocation = BindingLocation::BL_PARTICLE_BUFFER;
    mParticleBuffer.mActiveCount = NUM_PARTICLE_MAX;
    mParticleBuffer.mTotalCount = NUM_PARTICLE_MAX;
    mParticleBuffer.mBufferData.resize(mParticleBuffer.mTotalCount);
    GenerateAccretionDiskParticles();
    mParticleBuffer.Generate();

    mHashBuffer.mBindingLocation = BindingLocation::BL_SHG_HASH_BUFFER;
    mHashBuffer.mActiveCount = NUM_PARTICLE_MAX;
    mHashBuffer.mTotalCount = std::bit_ceil(mHashBuffer.mActiveCount); // increase to nearest powert of two for bitonic sort
    mHashBuffer.mBufferData.resize(mHashBuffer.mTotalCount);
    for(uint32_t i = 0; i < mHashBuffer.mTotalCount; i++)
    {
        mHashBuffer.mBufferData[i].hash = UINT_MAX;
        mHashBuffer.mBufferData[i].particleIndex = UINT_MAX;
    }
    for(uint32_t i = 0; i < mHashBuffer.mActiveCount; i++)
    {
        mHashBuffer.mBufferData[i].particleIndex = i;
    }    
    mHashBuffer.Generate();

    mCellFlagBuffer.mBindingLocation = BindingLocation::BL_SHG_CELL_FLAG_BUFFER;
    mCellFlagBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellFlagBuffer.mTotalCount = mHashBuffer.mTotalCount;
    mCellFlagBuffer.mBufferData.resize(mCellFlagBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellFlagBuffer.mTotalCount; i++)
    {
        mCellFlagBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellFlagBuffer.Generate();

    mCellPrefixBuffer.mBindingLocation = BindingLocation::BL_SHG_CELL_PREFIX_BUFFER;
    mCellPrefixBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellPrefixBuffer.mTotalCount = mHashBuffer.mTotalCount;
    mCellPrefixBuffer.mBufferData.resize(mCellPrefixBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellPrefixBuffer.mTotalCount; i++)
    {
        mCellPrefixBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellPrefixBuffer.Generate();

    mCellStartBuffer.mBindingLocation = BindingLocation::BL_SHG_START_BUFFER;
    mCellStartBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellStartBuffer.mTotalCount = mHashBuffer.mActiveCount;
    mCellStartBuffer.mBufferData.resize(mCellStartBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellStartBuffer.mTotalCount; i++)
    {
        mCellStartBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellStartBuffer.Generate();

    mCellEndBuffer.mBindingLocation = BindingLocation::BL_SHG_END_BUFFER;
    mCellEndBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellEndBuffer.mTotalCount = mHashBuffer.mActiveCount;
    mCellEndBuffer.mBufferData.resize(mCellEndBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellEndBuffer.mTotalCount; i++)
    {
        mCellEndBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellEndBuffer.Generate();

    mCellCountBuffer.mBindingLocation = BindingLocation::BL_SHG_CELL_COUNT_BUFFER;
    mCellCountBuffer.mActiveCount = 1;
    mCellCountBuffer.mTotalCount = 1;
    mCellCountBuffer.mBufferData.resize(1);
    mCellCountBuffer.mBufferData[0] = 0;
    mCellCountBuffer.Generate();

    mUniqueHashBuffer.mBindingLocation = BindingLocation::BL_SHG_UNIQUE_HASH_BUFFER;
    mUniqueHashBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mUniqueHashBuffer.mTotalCount = mHashBuffer.mActiveCount;
    mUniqueHashBuffer.mBufferData.resize(mUniqueHashBuffer.mTotalCount);
    mUniqueHashBuffer.Generate();

    mHashLookupBuffer.mBindingLocation = BindingLocation::BL_SHG_HASH_LOOKUP_BUFFER;
    mHashLookupBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mHashLookupBuffer.mTotalCount = mHashBuffer.mTotalCount;
    mHashLookupBuffer.mBufferData.resize(mHashLookupBuffer.mTotalCount);
    for(uint32_t i = 0; i < mHashLookupBuffer.mTotalCount; i++)
    {
        mHashLookupBuffer.mBufferData[i].hash = UINT_MAX;
        mHashLookupBuffer.mBufferData[i].cellIndex = UINT_MAX;
    }
    mHashLookupBuffer.Generate();
    
}

void SimulationManager::GenerateAccretionDiskParticles()
{

    if(BlackHole* blackHole = GetBlackHoleRefObject()){
        
        // spawn particles in relation to refernce object.
        for(Particle& particle : mParticleBuffer.mBufferData)
        {
            float randomAngle = SMath::RandomFloatByBounds(0, TWO_PI);
            float randomRadius = SMath::RandomFloatByBounds(blackHole->GetRadius() + mRadiusSpawnBounds);
            float randomZDeviation = SMath::RandomFloatByBounds(-0.05f, 0.05f);
            particle.mPosition = glm::vec3( blackHole->GetPosition().x + (randomRadius * cos(randomAngle)),
                                            blackHole->GetPosition().y + (randomRadius * sin(randomAngle)),
                                            blackHole->GetPosition().z + (randomZDeviation));

            particle.mRadius = mParticleRadius;
            
            // finding tangental vector by differentiating inital position rotation. x: cos -> -sin, y: sin -> cos
            float initalOrbitalVelocity = SMath::OrbitalVelocity(randomRadius, blackHole->GetMass());
            particle.mVelocity = glm::vec3(-sin(randomAngle) * initalOrbitalVelocity,
                                            cos(randomAngle) * initalOrbitalVelocity,
                                            0);

            particle.mMass = 0.f;
        }
    } else {
        // Spawn particles randomly near centre of world
        //for(Particle& particle : mParticleBuffer.mBufferData)
        //{
            //glm::vec3 min = glm::vec3(0.5f, -0.5f, -0.1f);
            //glm::vec3 max = glm::vec3(1.5f, 0.5f, 0.1f);
            //glm::vec3 newPosition = SMath::RandomVecByBounds(min,max);
            //particle.mPosition = glm::vec4(newPosition, 0.1f);
        //}
    }
}

void SimulationManager::CreateGravityPipeline()
{
    SimulationPipeline* gravityPipeline = new SimulationPipeline("Gravity Pipeline");

    ComputeShader* gravityComputeShader = new ComputeShader(PathUtil::shader_dir("accretionDiskGravSolver.comp"));
    std::vector<SSBOBinding> resources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId)};
    DispatchCallback DispatchCountCallback = [this](){ return mParticleBuffer.mActiveCount; };
    UniformCallback RefObjCallback = [this](Shader* _shader){ BindRefObjCallback(_shader); };
    UniformCallback DTCallback = [this](Shader* _shader){ BindDTCallback(_shader); };
    UniformCallback GCallback = [this](Shader* _shader){ BindGCallback(_shader); };
    std::vector<UniformCallback> uniforms = {RefObjCallback, DTCallback, GCallback};
    SimulationPass* pass = new SimulationPass(gravityComputeShader, resources, DispatchCountCallback, uniforms);
    gravityPipeline->AddPass(pass);

    mPipelines.push_back(gravityPipeline);
}

void SimulationManager::CreateSHGPipeline()
{
    SimulationPipeline* shgPipeline = new SimulationPipeline("SHG Pipeline");

    ComputeShader* shgHashComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_hash.comp"));
    std::vector<SSBOBinding> hashResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId), 
                                            SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId)};
    DispatchCallback DispatchCountCallbackHash = [this](){ return mHashBuffer.mTotalCount; };
    UniformCallback cellSizeCallbackHash = [this](Shader* _shader){ BindSHGCellSizeCallback(_shader); };
    UniformCallback ActiveCountCallbackHash = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsHash = {cellSizeCallbackHash, ActiveCountCallbackHash};
    SimulationPass* hashPass = new SimulationPass(shgHashComputeShader, hashResources, DispatchCountCallbackHash, uniformsHash);
    shgPipeline->AddPass(hashPass);

    ComputeShader* shgSortComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_bitonic_sort.comp"));
    std::vector<SSBOBinding> sortResources = {SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId)};
    DispatchCallback DispatchCountCallbackSort = [this](){ return mHashBuffer.mTotalCount; };
    UniformCallback cellSizeCallbackSort = [this](Shader* _shader){ BindSHGCellSizeCallback(_shader); };
    UniformCallback ActiveCountCallbackSort = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsSort = {cellSizeCallbackSort, ActiveCountCallbackSort};
    ExecuteCallback executeCallbackSort = [this](SimulationPass* _pass, uint32_t _count, uint32_t _groups){ BindCustomExecuet_BitonicSort(_pass,_count,_groups); };
    SimulationPass* sortPass = new SimulationPass(shgSortComputeShader, sortResources, DispatchCountCallbackSort, uniformsSort, executeCallbackSort);
    shgPipeline->AddPass(sortPass);

    ComputeShader* shgFlagComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_cell_flag.comp"));
    std::vector<SSBOBinding> flagResources = {SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId),
                                                SSBOBinding(mCellFlagBuffer.mBindingLocation, mCellFlagBuffer.mId),
                                                SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackFlag = [this](){ return mCellFlagBuffer.mTotalCount; };
    UniformCallback ActiveCountCallbackFlag = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsFlag = {ActiveCountCallbackFlag};
    SimulationPass* flagPass = new SimulationPass(shgFlagComputeShader, flagResources, DispatchCountCallbackFlag, uniformsFlag);
    shgPipeline->AddPass(flagPass);

    ComputeShader* shgUpScanComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_blelloch_up_scan.comp"));
    std::vector<SSBOBinding> UpScanResources = {SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackUpScan = [this](){ return mCellPrefixBuffer.mTotalCount; };
    std::vector<UniformCallback> uniformsUpScan = {};
    ExecuteCallback executeCallbackUpScan = [this](SimulationPass* _pass, uint32_t _count, uint32_t _groups){ BindCustomExecuet_BlellochScan_Up(_pass,_count,_groups); };
    SimulationPass* UpScanPass = new SimulationPass(shgUpScanComputeShader, UpScanResources, DispatchCountCallbackUpScan, uniformsUpScan, executeCallbackUpScan);
    shgPipeline->AddPass(UpScanPass);

    ComputeShader* shgResetComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_blelloch_reset.comp"));
    std::vector<SSBOBinding> ResetResources = {SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId),
                                                SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackReset = [this](){ return 1; };
    std::vector<UniformCallback> uniformsReset = {};
    SimulationPass* ResetPass = new SimulationPass(shgResetComputeShader, ResetResources, DispatchCountCallbackReset, uniformsReset);
    shgPipeline->AddPass(ResetPass);

    ComputeShader* shgDownScanComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_blelloch_down_scan.comp"));
    std::vector<SSBOBinding> DownScanResources = {SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackDownScan = [this](){ return mCellPrefixBuffer.mTotalCount; };
    std::vector<UniformCallback> uniformsDownScan = {};
    ExecuteCallback executeCallbackDownScan = [this](SimulationPass* _pass, uint32_t _count, uint32_t _groups){ BindCustomExecuet_BlellochScan_Down(_pass,_count,_groups); };
    SimulationPass* DownScanPass = new SimulationPass(shgDownScanComputeShader, DownScanResources, DispatchCountCallbackDownScan, uniformsDownScan, executeCallbackDownScan);
    shgPipeline->AddPass(DownScanPass);

    ComputeShader* shgRangeStartComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_range_start.comp"));
    std::vector<SSBOBinding> rangeStartResources = {SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId),
                                                SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                                SSBOBinding(mUniqueHashBuffer.mBindingLocation, mUniqueHashBuffer.mId),
                                                SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackRangeStart = [this](){ return mParticleBuffer.mActiveCount; };
    std::vector<UniformCallback> uniformsRangeStart = {};
    SimulationPass* rangeStartPass = new SimulationPass(shgRangeStartComputeShader, rangeStartResources, DispatchCountCallbackRangeStart, uniformsRangeStart);
    shgPipeline->AddPass(rangeStartPass);

    ComputeShader* shgRangeEndComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_range_end.comp"));
    std::vector<SSBOBinding> rangeEndResources = {SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                                SSBOBinding(mCellEndBuffer.mBindingLocation, mCellEndBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackRangeEnd = [this](){ return mCellEndBuffer.mActiveCount; };
    UniformCallback ActiveCountCallbackRangeEnd = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsRangeEnd = {ActiveCountCallbackRangeEnd};
    SimulationPass* rangeEndPass = new SimulationPass(shgRangeEndComputeShader, rangeEndResources, DispatchCountCallbackRangeEnd, uniformsRangeEnd);
    shgPipeline->AddPass(rangeEndPass);

    ComputeShader* shgLookUpComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_build_lookup_table.comp"));
    std::vector<SSBOBinding> LookUpResources = {SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId),
                                                SSBOBinding(mUniqueHashBuffer.mBindingLocation, mUniqueHashBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackLookUp = [this](){ return mCellCountBuffer.ReadBack()[0]; };
    std::vector<UniformCallback> uniformsLookUp = {};
    SimulationPass* LookUpPass = new SimulationPass(shgLookUpComputeShader, LookUpResources, DispatchCountCallbackLookUp, uniformsLookUp);
    shgPipeline->AddPass(LookUpPass);

    mPipelines.push_back(shgPipeline);
}

void SimulationManager::CreateSPHPipeline()
{
    // SimulationPipeline* sphPipeline = new SimulationPipeline("SPH Pipeline");

    // ComputeShader* sphComputeShader = new ComputeShader(PathUtil::shader_dir("accretionDiskSPHSolver.comp"));
    // std::vector<SSBOBinding> resources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId)};
    // DispatchCallback DispatchCountCallback = [this](){ return mParticleBuffer.mActiveCount; };
    // UniformCallback RefObjCallback = [this](Shader* _shader){ BindRefObjCallback(_shader); };
    // std::vector<UniformCallback> uniforms = {RefObjCallback};
    // SimulationPass* pass = new SimulationPass(sphComputeShader, resources, DispatchCountCallback, uniforms);
    // sphPipeline->AddPass(pass);

    // mPipelines.push_back(sphPipeline);
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

// ToDo:Update function to take in relative active count based on task, for now it is hardcoded to particles
void SimulationManager::BindActiveCountCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setInt("activeCount", mParticleBuffer.mActiveCount);
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

void SimulationManager::BindSHGCellSizeCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setFloat("cellSize", gCellSize);
}

void SimulationManager::BindCustomExecuet_BitonicSort(class SimulationPass* _pass, uint32_t _count, uint32_t _groups)
{
    if(!std::has_single_bit(_count)){
        LOG_ERROR("Bitonic Sort requires number of invocations as a power of two. Shader pass not run");
        return;
    }

    for(uint32_t k = 2; k <= _count; k <<= 1)
    {
        for(uint32_t j = k >> 1; j > 0; j >>= 1)
        {
            _pass->GetShader()->use();
            _pass->GetShader()->setInt("k", k);
            _pass->GetShader()->setInt("j", j);

            _pass->Dispatch(_groups);
        }
    }
}

void SimulationManager::BindCustomExecuet_BlellochScan_Up(class SimulationPass* _pass, uint32_t _count, uint32_t _groups)
{
    for(int stride = 1; stride < _count; stride <<= 1)
    {
        uint32_t localSize = _pass->GetWorkGroupSize();
        uint32_t invocations = _count / (stride * 2);
        uint32_t groups = (invocations + localSize - 1) / localSize;

        _pass->GetShader()->use();
        _pass->GetShader()->setInt("stride", stride);

        _pass->Dispatch(groups);
    }
}

void SimulationManager::BindCustomExecuet_BlellochScan_Down(class SimulationPass* _pass, uint32_t _count, uint32_t _groups)
{
    for(int stride = _count/2; stride >= 1; stride >>=1)
    {
        uint32_t localSize = _pass->GetWorkGroupSize();
        uint32_t invocations = _count / (stride * 2);
        uint32_t groups = (invocations + localSize - 1) / localSize;

        _pass->GetShader()->use();
        _pass->GetShader()->setInt("stride", stride);

        _pass->Dispatch(groups);
    }
}

BlackHole* SimulationManager::GetBlackHoleRefObject()
{
    int counter = 0;
    BlackHole* bh = nullptr;
    for(Object* obj : *mReferenceObjectsRef){
        if(obj){
            if(BlackHole* blackHole = dynamic_cast<BlackHole*>(obj)){
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

std::vector<std::pair<const char*, float>> SimulationManager::GetSimulationUIData()
{
    std::vector<std::pair<const char*, float>> passTime;
    for(SimulationPipeline* pipeline : mPipelines){
        pipeline->GetUIData(passTime);
    }

    return passTime;
}