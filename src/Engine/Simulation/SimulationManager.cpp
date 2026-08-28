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
    CreateDGPipeline();
}

void SimulationManager::Update(float _dt)
{
    for(SimulationPipeline* pipline : mPipelines){
        pipline->ExecutePipeline();
    }

    //LOG_WARNING("CellEntries: ");
    //auto grid = ReadBuffer(mRenderGridBuffer);
    //for(uint32_t i = 0; i < grid.size(); i++)
    //{
    //    LOG_DEBUG("Index: {}, cellIndex: {}, particleIndex: {}", i, grid[i].mHash, grid[i].mIndex);
    //}

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

    // LOG_WARNING("Flag buffer: ");
    // auto flag = ReadBuffer(mRenderCellFlagBuffer);
    // for(uint32_t i = 0; i < flag.size(); i++)
    // {
    //     LOG_DEBUG("Index: {}, flag: {}", i, flag[i]);
    // }
   
    // LOG_WARNING("Prefix buffer: ");
    // auto prefix = ReadBuffer(mCellPrefixBuffer);
    // for(uint32_t i = 0; i < prefix.size(); i++)
    // {
    //     LOG_DEBUG("Inex: {}, prefix: {}", i, prefix[i]);
    // }
    // LOG_WARNING("Prefix buffer: ");
    // auto prefix = ReadBuffer(mRenderCellPrefixBuffer);
    // for(uint32_t i = 0; i < prefix.size(); i++)
    // {
    //     LOG_DEBUG("Index: {}, prefix: {}", i, prefix[i]);
    // }
    // LOG_WARNING("End");
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
    
    // LOG_WARNING("Lookuptable: ");
    // auto table = ReadBuffer(mHashLookupBuffer);
    // for(uint32_t i = 0; i < table.size(); i++)
    // {
    //     LOG_DEBUG("Index: {}, Hash: {}, cell: {}", i, table[i].hash, table[i].cellIndex);
    // }
    //LOG_DEBUG("End");
}

void SimulationManager::BindBuffer(GLint _bufferID, int _layout)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _layout, _bufferID);
}

void SimulationManager::InitBuffers()
{
    mParticleBuffer.mBindingLocation = BindingLocation::BL_PARTICLE_BUFFER;
    mParticleBuffer.mActiveCount = NUM_PARTICLE_MAX;
    mParticleBuffer.mTotalCount = NUM_PARTICLE_MAX;
    mParticleBuffer.mBufferData.resize(mParticleBuffer.mTotalCount);
    GenerateAccretionDiskParticles();
    mParticleBuffer.Generate();

    mHashBuffer.mBindingLocation = BindingLocation::BL_HASH_ENTRY_BUFFER;
    mHashBuffer.mActiveCount = NUM_PARTICLE_MAX;
    mHashBuffer.mTotalCount = std::bit_ceil(mHashBuffer.mActiveCount); // increase to nearest powert of two for bitonic sort
    mHashBuffer.mBufferData.resize(mHashBuffer.mTotalCount);
    for(uint32_t i = 0; i < mHashBuffer.mTotalCount; i++)
    {
        mHashBuffer.mBufferData[i].mHash = UINT_MAX;
        mHashBuffer.mBufferData[i].mIndex = UINT_MAX;
    }
    for(uint32_t i = 0; i < mHashBuffer.mActiveCount; i++)
    {
        mHashBuffer.mBufferData[i].mIndex = i;
    }    
    mHashBuffer.Generate();

    mCellFlagBuffer.mBindingLocation = BindingLocation::BL_FLAG_BUFFER;
    mCellFlagBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellFlagBuffer.mTotalCount = mHashBuffer.mTotalCount;
    mCellFlagBuffer.mBufferData.resize(mCellFlagBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellFlagBuffer.mTotalCount; i++)
    {
        mCellFlagBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellFlagBuffer.Generate();

    mCellPrefixBuffer.mBindingLocation = BindingLocation::BL_PREFIX_BUFFER;
    mCellPrefixBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellPrefixBuffer.mTotalCount = mHashBuffer.mTotalCount;
    mCellPrefixBuffer.mBufferData.resize(mCellPrefixBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellPrefixBuffer.mTotalCount; i++)
    {
        mCellPrefixBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellPrefixBuffer.Generate();

    mCellCountBuffer.mBindingLocation = BindingLocation::BL_COUNT_BUFFER;
    mCellCountBuffer.mActiveCount = 1;
    mCellCountBuffer.mTotalCount = 1;
    mCellCountBuffer.mBufferData.resize(1);
    mCellCountBuffer.mBufferData[0] = 0;
    mCellCountBuffer.Generate();

    mCellStartBuffer.mBindingLocation = BindingLocation::BL_RANGE_START_BUFFER;
    mCellStartBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellStartBuffer.mTotalCount = mHashBuffer.mActiveCount;
    mCellStartBuffer.mBufferData.resize(mCellStartBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellStartBuffer.mTotalCount; i++)
    {
        mCellStartBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellStartBuffer.Generate();

    mCellEndBuffer.mBindingLocation = BindingLocation::BL_RANGE_END_BUFFER;
    mCellEndBuffer.mActiveCount = mHashBuffer.mActiveCount;
    mCellEndBuffer.mTotalCount = mHashBuffer.mActiveCount;
    mCellEndBuffer.mBufferData.resize(mCellEndBuffer.mTotalCount);
    for(uint32_t i = 0; i < mCellEndBuffer.mTotalCount; i++)
    {
        mCellEndBuffer.mBufferData[i] = UINT_MAX;
    }
    mCellEndBuffer.Generate();

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
        mHashLookupBuffer.mBufferData[i].mHash = UINT_MAX;
        mHashLookupBuffer.mBufferData[i].mIndex = UINT_MAX;
    }
    mHashLookupBuffer.Generate();
    
    // RENDERING

    mRenderGridBuffer.mBindingLocation = BindingLocation::BL_HASH_ENTRY_BUFFER;
    mRenderGridBuffer.mActiveCount = mParticleBuffer.mActiveCount;
    mRenderGridBuffer.mTotalCount = std::bit_ceil(mRenderGridBuffer.mActiveCount); // increase to nearest powert of two for bitonic sort;
    mRenderGridBuffer.mBufferData.resize(mRenderGridBuffer.mTotalCount);
    for(uint32_t i = 0; i < mRenderGridBuffer.mTotalCount; i++)
    {
        mRenderGridBuffer.mBufferData[i].mHash = UINT_MAX;
        mRenderGridBuffer.mBufferData[i].mIndex = UINT_MAX;
    }
    mRenderGridBuffer.Generate();

    mRenderCellFlagBuffer.mBindingLocation = BindingLocation::BL_FLAG_BUFFER;
    mRenderCellFlagBuffer.mActiveCount = mRenderGridBuffer.mActiveCount;
    mRenderCellFlagBuffer.mTotalCount = mRenderGridBuffer.mTotalCount;
    mRenderCellFlagBuffer.mBufferData.resize(mRenderCellFlagBuffer.mTotalCount);
    for(uint32_t i = 0; i < mRenderCellFlagBuffer.mTotalCount; i++)
    {
        mRenderCellFlagBuffer.mBufferData[i] = UINT_MAX;
    }
    mRenderCellFlagBuffer.Generate();

    mRenderCellPrefixBuffer.mBindingLocation = BindingLocation::BL_PREFIX_BUFFER;
    mRenderCellPrefixBuffer.mActiveCount = mRenderGridBuffer.mActiveCount;
    mRenderCellPrefixBuffer.mTotalCount = mRenderGridBuffer.mTotalCount;
    mRenderCellPrefixBuffer.mBufferData.resize(mRenderCellPrefixBuffer.mTotalCount);
    for(uint32_t i = 0; i < mRenderCellPrefixBuffer.mTotalCount; i++)
    {
        mRenderCellPrefixBuffer.mBufferData[i] = UINT_MAX;
    }
    mRenderCellPrefixBuffer.Generate();

    mRenderCellCountBuffer.mBindingLocation = BindingLocation::BL_COUNT_BUFFER;
    mRenderCellCountBuffer.mActiveCount = 1;
    mRenderCellCountBuffer.mTotalCount = 1;
    mRenderCellCountBuffer.mBufferData.resize(1);
    mRenderCellCountBuffer.mBufferData[0] = 0;
    mRenderCellCountBuffer.Generate();

    mRenderCellStartBuffer.mBindingLocation = BindingLocation::BL_RANGE_START_BUFFER;
    mRenderCellStartBuffer.mActiveCount = gCellCount;
    mRenderCellStartBuffer.mTotalCount = gCellCount;
    mRenderCellStartBuffer.mBufferData.resize(gCellCount, UINT_MAX);
    mRenderCellStartBuffer.Generate();

    mRenderCellEndBuffer.mBindingLocation = BindingLocation::BL_RANGE_END_BUFFER;
    mRenderCellEndBuffer.mActiveCount = gCellCount;
    mRenderCellEndBuffer.mTotalCount = gCellCount;
    mRenderCellEndBuffer.mBufferData.resize(gCellCount, UINT_MAX);
    mRenderCellEndBuffer.Generate();
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

            particle.mRadius = gParticleRadius;
            
            // finding tangental vector by differentiating inital position rotation. x: cos -> -sin, y: sin -> cos
            float initalOrbitalVelocity = SMath::OrbitalVelocity(randomRadius, blackHole->GetMass());
            particle.mVelocity = glm::vec3(-sin(randomAngle) * initalOrbitalVelocity,
                                            cos(randomAngle) * initalOrbitalVelocity,
                                            0);

            particle.mMass = 0.f;
            particle.mColor = glm::vec4(1.f,0.f,0,1.f);
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
    DispatchCallback DispatchCountCallback = [this](){ return glm::ivec3(mParticleBuffer.mActiveCount,0,0); };
    UniformCallback RefObjCallback = [this](Shader* _shader){ BindRefObjCallback(_shader); };
    UniformCallback dtCallback = [this](Shader* _shader){ BindDTCallback(_shader); };
    UniformCallback GCallback = [this](Shader* _shader){ BindGCallback(_shader); };
    std::vector<UniformCallback> uniforms = {RefObjCallback, dtCallback, GCallback};
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
    DispatchCallback DispatchCountCallbackHash = [this](){ return glm::ivec3(mHashBuffer.mTotalCount,1,1); };
    UniformCallback cellSizeCallbackHash = [this](Shader* _shader){ BindCellSizeCallback(_shader); };
    UniformCallback ActiveCountCallbackHash = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsHash = {cellSizeCallbackHash, ActiveCountCallbackHash};
    SimulationPass* hashPass = new SimulationPass(shgHashComputeShader, hashResources, DispatchCountCallbackHash, uniformsHash);
    shgPipeline->AddPass(hashPass);

    CreateBitonicSortPass(shgPipeline, mHashBuffer);
    CreateFlagPass(shgPipeline, mHashBuffer, mCellFlagBuffer, mCellPrefixBuffer);
    CreateBlellochScanPass(shgPipeline, mCellPrefixBuffer, mCellCountBuffer);

    ComputeShader* shgRangeStartComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_range_start.comp"));
    std::vector<SSBOBinding> rangeStartResources = {SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId),
                                                SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                                SSBOBinding(mUniqueHashBuffer.mBindingLocation, mUniqueHashBuffer.mId),
                                                SSBOBinding(mCellPrefixBuffer.mBindingLocation, mCellPrefixBuffer.mId)};
    DispatchCallback DispatchCountCallbackRangeStart = [this](){ return glm::ivec3(mParticleBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> uniformsRangeStart = {};
    SimulationPass* rangeStartPass = new SimulationPass(shgRangeStartComputeShader, rangeStartResources, DispatchCountCallbackRangeStart, uniformsRangeStart);
    shgPipeline->AddPass(rangeStartPass);

    ComputeShader* shgRangeEndComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_range_end.comp"));
    std::vector<SSBOBinding> rangeEndResources = {SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                                SSBOBinding(mCellEndBuffer.mBindingLocation, mCellEndBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackRangeEnd = [this](){ return glm::ivec3(mCellEndBuffer.mActiveCount,1,1); };
    UniformCallback ActiveCountCallbackRangeEnd = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsRangeEnd = {ActiveCountCallbackRangeEnd};
    SimulationPass* rangeEndPass = new SimulationPass(shgRangeEndComputeShader, rangeEndResources, DispatchCountCallbackRangeEnd, uniformsRangeEnd);
    shgPipeline->AddPass(rangeEndPass);

    ComputeShader* shgResetLookUpComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_reset_lookup_table.comp"));
    std::vector<SSBOBinding> ResetLookUpResources = {SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId)};
    DispatchCallback DispatchCountCallbackResetLookUp = [this](){ return glm::ivec3(mCellCountBuffer.ReadBack()[0],1,1); };
    std::vector<UniformCallback> uniformsResetLookUp = {};
    SimulationPass* ResetLookUpPass = new SimulationPass(shgResetLookUpComputeShader, ResetLookUpResources, DispatchCountCallbackResetLookUp, uniformsResetLookUp);
    shgPipeline->AddPass(ResetLookUpPass);

    ComputeShader* shgLookUpComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_build_lookup_table.comp"));
    std::vector<SSBOBinding> LookUpResources = {SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId),
                                                SSBOBinding(mUniqueHashBuffer.mBindingLocation, mUniqueHashBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackLookUp = [this](){ return glm::ivec3(mCellCountBuffer.ReadBack()[0],1,1); };
    std::vector<UniformCallback> uniformsLookUp = {};
    SimulationPass* LookUpPass = new SimulationPass(shgLookUpComputeShader, LookUpResources, DispatchCountCallbackLookUp, uniformsLookUp);
    shgPipeline->AddPass(LookUpPass);

    mPipelines.push_back(shgPipeline);
}

void SimulationManager::CreateSPHPipeline()
{
    SimulationPipeline* sphPipeline = new SimulationPipeline("SPH Pipeline");

    ComputeShader* sphComputeShader = new ComputeShader(PathUtil::shader_dir("sph_particle.comp"));
    std::vector<SSBOBinding> sphResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId),
                                            SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId),
                                            SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                            SSBOBinding(mCellEndBuffer.mBindingLocation, mCellEndBuffer.mId),
                                            SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId)};
    DispatchCallback sphDispatchCountCallback = [this](){ return glm::ivec3(mParticleBuffer.mActiveCount,1,1); };
    UniformCallback sphCellSizeCallback = [this](Shader* _shader){ BindCellSizeCallback(_shader); };
    UniformCallback sphSmoothingRadiusCallback = [this](Shader* _shader){ BindSPHSmoothingRadius(_shader); };
    std::vector<UniformCallback> sphUniforms = {sphCellSizeCallback, sphSmoothingRadiusCallback};
    SimulationPass* sphPass = new SimulationPass(sphComputeShader, sphResources, sphDispatchCountCallback, sphUniforms);
    sphPipeline->AddPass(sphPass);
    
    mPipelines.push_back(sphPipeline);
}

void SimulationManager::CreateDGPipeline()
{
    SimulationPipeline* dgPipeline = new SimulationPipeline("Dense Grid Pipeline");

    ComputeShader* gridHashComputeShader = new ComputeShader(PathUtil::shader_dir("DenseGrid/dg_grid_hash.comp"));
    std::vector<SSBOBinding> gridHashResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId),
                                                SSBOBinding(mRenderGridBuffer.mBindingLocation, mRenderGridBuffer.mId)};
    DispatchCallback gridHashDispatchCountCallback = [this](){ return glm::ivec3(mRenderGridBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> gridHashUniforms = {[this](Shader* _shader){ BindCallback(_shader, "cellSize", gCellSize); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "gridMin", gGridBoundsMin); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "gridSize", gGridSize); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "activeCount", int(mRenderGridBuffer.mActiveCount)); }};
    SimulationPass* gridHashPass = new SimulationPass(gridHashComputeShader, gridHashResources, gridHashDispatchCountCallback, gridHashUniforms);
    dgPipeline->AddPass(gridHashPass);

    CreateBitonicSortPass(dgPipeline, mRenderGridBuffer);
    CreateFlagPass(dgPipeline, mRenderGridBuffer, mRenderCellFlagBuffer, mRenderCellPrefixBuffer);
    CreateBlellochScanPass(dgPipeline, mRenderCellPrefixBuffer, mRenderCellCountBuffer);

    ComputeShader* cellRangeComputeShader = new ComputeShader(PathUtil::shader_dir("DenseGrid/dg_range_start.comp"));
    std::vector<SSBOBinding> cellRangeResources = {SSBOBinding(mRenderGridBuffer.mBindingLocation, mRenderGridBuffer.mId),
                                                SSBOBinding(mRenderCellStartBuffer.mBindingLocation, mRenderCellStartBuffer.mId),
                                                SSBOBinding(mRenderCellEndBuffer.mBindingLocation, mRenderCellEndBuffer.mId),
                                                SSBOBinding(mRenderCellFlagBuffer.mBindingLocation, mRenderCellFlagBuffer.mId),
                                                SSBOBinding(mRenderCellPrefixBuffer.mBindingLocation, mRenderCellPrefixBuffer.mId)};
    DispatchCallback cellRangeDispatchCountCallback = [this](){ return glm::ivec3(mRenderCellStartBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> cellRangeUniforms = {[this](Shader* _shader){ BindCallback(_shader, "activeCount", int(mRenderGridBuffer.mActiveCount)); }};
    SimulationPass* cellRangePass = new SimulationPass(cellRangeComputeShader, cellRangeResources, cellRangeDispatchCountCallback, cellRangeUniforms);
    dgPipeline->AddPass(cellRangePass);

    mPipelines.push_back(dgPipeline);
}

void SimulationManager::CreateBitonicSortPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer)
{
    ComputeShader* sortComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_bitonic_sort.comp"));
    std::vector<SSBOBinding> sortResources = {SSBOBinding(_gridBuffer.mBindingLocation, _gridBuffer.mId)};
    DispatchCallback sortDispatchCountCallback = [this, &_gridBuffer](){ return glm::ivec3(_gridBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> sortUniforms = {};
    ExecuteCallback sortExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BitonicSort(_pass,_count,_groups); };
    SimulationPass* sortPass = new SimulationPass(sortComputeShader, sortResources, sortDispatchCountCallback, sortUniforms, sortExecuteCallback);
    _pipeline->AddPass(sortPass);
}

void SimulationManager::CreateFlagPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer, GPUBuffer<uint32_t>& _flagBuffer, GPUBuffer<uint32_t>& _prefixBuffer)
{
    ComputeShader* computeShader = new ComputeShader(PathUtil::shader_dir("universal/u_hash_entry_flag.comp"));
    std::vector<SSBOBinding> resources = {SSBOBinding(_gridBuffer.mBindingLocation, _gridBuffer.mId),
                                            SSBOBinding(_flagBuffer.mBindingLocation, _flagBuffer.mId),
                                            SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback dispatchCountCallback = [this, &_flagBuffer](){ return glm::ivec3(int(_flagBuffer.mTotalCount), 1, 1); };
    std::vector<UniformCallback> uniforms = {[this, &_flagBuffer](Shader* _shader){ BindCallback(_shader, "activeCount", int(_flagBuffer.mActiveCount)); }};
    SimulationPass* pass = new SimulationPass(computeShader, resources, dispatchCountCallback, uniforms);
    _pipeline->AddPass(pass);
}

void SimulationManager::CreateBlellochScanPass(SimulationPipeline* _pipeline, GPUBuffer<uint32_t>& _prefixBuffer, GPUBuffer<uint32_t>& _countBuffer)
{
    ComputeShader* upScanComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_up_scan.comp"));
    std::vector<SSBOBinding> upScanResources = {SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback upScanDispatchCountCallback = [this, &_prefixBuffer](){ return glm::ivec3(_prefixBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> upScanUniforms = {};
    ExecuteCallback upScanExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BlellochScan_Up(_pass,_count,_groups); };
    SimulationPass* upScanPass = new SimulationPass(upScanComputeShader, upScanResources, upScanDispatchCountCallback, upScanUniforms, upScanExecuteCallback);
    _pipeline->AddPass(upScanPass);

    ComputeShader* resetComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_reset.comp"));
    std::vector<SSBOBinding> resetResources = {SSBOBinding(_countBuffer.mBindingLocation, _countBuffer.mId),
                                                SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback resetDispatchCountCallback = [this](){ return glm::ivec3(1,1,1); };
    std::vector<UniformCallback> resetUniforms = {};
    SimulationPass* resetPass = new SimulationPass(resetComputeShader, resetResources, resetDispatchCountCallback, resetUniforms);
    _pipeline->AddPass(resetPass);

    ComputeShader* downScanComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_down_scan.comp"));
    std::vector<SSBOBinding> downScanResources = {SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback downScanDispatchCountCallback = [this, &_prefixBuffer](){ return glm::ivec3(_prefixBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> downScanUniforms = {};
    ExecuteCallback downScanExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BlellochScan_Down(_pass,_count,_groups); };
    SimulationPass* downScanPass = new SimulationPass(downScanComputeShader, downScanResources, downScanDispatchCountCallback, downScanUniforms, downScanExecuteCallback);
    _pipeline->AddPass(downScanPass);
}

void SimulationManager::BindCallback(class Shader* _shader, const char* _varName, unsigned int _val)
{
    _shader->use();
    _shader->setUInt(_varName, _val);
}

void SimulationManager::BindCallback(class Shader* _shader, const char* _varName, int _val)
{
    _shader->use();
    _shader->setInt(_varName, _val);
}

void SimulationManager::BindCallback(class Shader* _shader, const char* _varName, float _val)
{
    _shader->use();
    _shader->setFloat(_varName, _val);
}

void SimulationManager::BindCallback(class Shader* _shader, const char* _varName, glm::ivec3 _val)
{
    _shader->use();
    _shader->setIVec3(_varName, _val);
}

void SimulationManager::BindCallback(class Shader* _shader, const char* _varName, glm::vec3 _val)
{
    _shader->use();
    _shader->setVec3(_varName, _val);
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

void SimulationManager::BindCellSizeCallback(class Shader* _shader)
{
    _shader->use();
    _shader->setFloat("cellSize", gCellSize);
}

void  SimulationManager::BindSPHSmoothingRadius(class Shader* _shader)
{
    _shader->use();
    _shader->setFloat("smoothingRadius", gSmoothingRadius);
}

std::vector<SSBOBinding> SimulationManager::GetRaytracerResources()
{
    return {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId),
            SSBOBinding(mRenderGridBuffer.mBindingLocation, mRenderGridBuffer.mId),
            SSBOBinding(mRenderCellStartBuffer.mBindingLocation, mRenderCellStartBuffer.mId),
            SSBOBinding(mRenderCellEndBuffer.mBindingLocation, mRenderCellEndBuffer.mId)};
}

void SimulationManager::BindCustomExecuet_BitonicSort(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups)
{
    if(!std::has_single_bit(unsigned int(_count.x))){
        LOG_ERROR("Bitonic Sort requires number of invocations as a power of two. Shader pass not run");
        return;
    }

    for(uint32_t k = 2; k <= _count.x; k <<= 1)
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

void SimulationManager::BindCustomExecuet_BlellochScan_Up(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups)
{
    for(int stride = 1; stride < _count.x; stride <<= 1)
    {
        uint32_t invocations = _count.x / (stride * 2);
        glm::ivec3 groups = _pass->NumGroups(glm::ivec3(invocations,0,0));

        _pass->GetShader()->use();
        _pass->GetShader()->setInt("stride", stride);

        _pass->Dispatch(groups);
    }
}

void SimulationManager::BindCustomExecuet_BlellochScan_Down(class SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups)
{
    for(int stride = _count.x/2; stride >= 1; stride >>=1)
    {
        uint32_t invocations = _count.x / (stride * 2);
        glm::ivec3 groups = _pass->NumGroups(glm::ivec3(invocations,0,0));

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