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

    //ValidateHashGrid();
    //ValidateRenderGrid();
    //LOG_WARNING("Particles: ");
    // auto particles = ReadBuffer(mParticleBuffer);
    // for(uint32_t i = 0; i < particles.size(); i++)
    // {
    //     LOG_DEBUG("Index: {}, particle pos .x: {} .y:{} .z;{}", i, particles[i].mPosition.x, particles[i].mPosition.y, particles[i].mPosition.z);
    // }
    // auto particles = ReadBuffer(mParticleBuffer);
    // auto entries   = ReadBuffer(mRenderGridBuffer);
    // auto starts    = ReadBuffer(mRenderCellStartBuffer);
    // auto ends      = ReadBuffer(mRenderCellEndBuffer);

    // auto flags = ReadBuffer(mRenderCellFlagBuffer);

    // uint32_t validRanges = 0;
    // uint32_t maxEnd = 0;

    // for (uint32_t cell = 0; cell < gCellCount; ++cell)
    // {
    //     if (starts[cell] == UINT_MAX && ends[cell] == UINT_MAX)
    //         continue;

    //     LOG_DEBUG(
    //         "RANGE cell={} start={} end={}",
    //         cell,
    //         starts[cell],
    //         ends[cell]
    //     );

    //     validRanges++;
    //     maxEnd = std::max(maxEnd, ends[cell]);
    // }

    // LOG_DEBUG("Valid ranges = {}, maxEnd = {}", validRanges, maxEnd);

    // uint32_t flagCount = 0;

    // for (uint32_t i = 0; i < 3000; ++i)
    //     flagCount += flags[i];

    // LOG_DEBUG("FLAG COUNT = {}", flagCount);

    // LOG_DEBUG("End1");

    // for (uint32_t i = 0; i < 30; ++i)
    // {
    //     LOG_DEBUG(
    //         "FLAG[{}] hash={} flag={}",
    //         i,
    //         entries[i].mHash,
    //         flags[i]
    //     );
    // }
    // LOG_DEBUG("End2");

    // uint32_t zCount[10] = {};

    // for (uint32_t i = 0; i < particles.size(); ++i)
    // {
    //     glm::ivec3 cell = glm::ivec3(
    //         glm::floor(
    //             (particles[i].mPosition - gGridBoundsMin) / gCellSize
    //         )
    //     );

    //     if (cell.x < 0 || cell.x >= 10 ||
    //         cell.y < 0 || cell.y >= 10 ||
    //         cell.z < 0 || cell.z >= 10)
    //         continue;

    //     zCount[cell.z]++;
    // }
    
    // for (int z = 0; z < 10; ++z)
    //     LOG_DEBUG("Z layer {}: {} particles", z, zCount[z]);

    // LOG_DEBUG("End3");
    // uint32_t total = 0;
    // for (uint32_t cell = 0; cell < gCellCount; ++cell)
    // {
    //     if (starts[cell] == UINT_MAX ||
    //         ends[cell] == UINT_MAX)
    //         continue;

    //     total += ends[cell] - starts[cell];
    // }

    // LOG_DEBUG("Particles represented by ranges: {}", total);
    // LOG_DEBUG("End4");

    // for (uint32_t cell = 0; cell < gCellCount; ++cell)
    // {
    //     uint32_t start = starts[cell];
    //     uint32_t end = ends[cell];

    //     if (start == UINT_MAX)
    //         continue;

    //     for (uint32_t i = start; i < end; ++i)
    //     {
    //         uint32_t particle = entries[i].mIndex;
            
    //         if (particle == UINT_MAX || particle >= particles.size())
    //         {
    //             LOG_DEBUG(
    //                 "BAD PARTICLE INDEX: cell={} i={} particle={}",
    //                 cell, i, particle
    //             );
    //             continue;
    //         }

    //         // Calculate expected cell on CPU
    //         glm::ivec3 expectedCell = glm::ivec3(
    //             glm::floor(
    //                 (particles[particle].mPosition - gGridBoundsMin)
    //                 / gCellSize
    //             )
    //         );

    //         uint32_t expectedIndex =
    //             expectedCell.x +
    //             expectedCell.y * gGridSize.x +
    //             expectedCell.z * gGridSize.x * gGridSize.y;

    //         if (expectedIndex != cell)
    //         {
    //             LOG_DEBUG(
    //                 "BAD RANGE: cell={} particle={} expectedCell={}",
    //                 cell,
    //                 particle,
    //                 expectedIndex
    //             );
    //         }
    //     }
    // }
    // LOG_DEBUG("End5");
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
    mHashLookupBuffer.mTotalCount = std::bit_ceil(mHashBuffer.mActiveCount * 2);
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
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;

    ComputeShader* gravityComputeShader = new ComputeShader(PathUtil::shader_dir("accretionDiskGravSolver.comp"));
    std::vector<SSBOBinding> resources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId)};
    DispatchCallback DispatchCountCallback = [this](){ return glm::ivec3(mParticleBuffer.mActiveCount,0,0); };
    UniformCallback RefObjCallback = [this](Shader* _shader){ BindRefObjCallback(_shader); };
    UniformCallback dtCallback = [this](Shader* _shader){ BindDTCallback(_shader); };
    UniformCallback GCallback = [this](Shader* _shader){ BindGCallback(_shader); };
    std::vector<UniformCallback> uniforms = {RefObjCallback, dtCallback, GCallback};
    SimulationPass* pass = new SimulationPass(gravityComputeShader, resources, DispatchCountCallback, uniforms, passFlag);
    gravityPipeline->AddPass(pass);

    mPipelines.push_back(gravityPipeline);
}

void SimulationManager::CreateSHGPipeline()
{
    SimulationPipeline* shgPipeline = new SimulationPipeline("SHG Pipeline");
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;

    ComputeShader* shgHashComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_hash.comp"));
    std::vector<SSBOBinding> hashResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId), 
                                            SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId)};
    DispatchCallback DispatchCountCallbackHash = [this](){ return glm::ivec3(mHashBuffer.mTotalCount,1,1); };
    UniformCallback cellSizeCallbackHash = [this](Shader* _shader){ BindCellSizeCallback(_shader); };
    UniformCallback ActiveCountCallbackHash = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    UniformCallback GridMinCallback = [this](Shader* _shader){ BindCallback(_shader, "gridMin", gGridBoundsMin); };
    std::vector<UniformCallback> uniformsHash = {cellSizeCallbackHash, ActiveCountCallbackHash, GridMinCallback};
    SimulationPass* hashPass = new SimulationPass(shgHashComputeShader, hashResources, DispatchCountCallbackHash, uniformsHash, passFlag);
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
    SimulationPass* rangeStartPass = new SimulationPass(shgRangeStartComputeShader, rangeStartResources, DispatchCountCallbackRangeStart, uniformsRangeStart, passFlag);
    shgPipeline->AddPass(rangeStartPass);

    ComputeShader* shgRangeEndComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_range_end.comp"));
    std::vector<SSBOBinding> rangeEndResources = {SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                                SSBOBinding(mCellEndBuffer.mBindingLocation, mCellEndBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackRangeEnd = [this](){ return glm::ivec3(mCellEndBuffer.mActiveCount,1,1); };
    UniformCallback ActiveCountCallbackRangeEnd = [this](Shader* _shader){ BindActiveCountCallback(_shader); };
    std::vector<UniformCallback> uniformsRangeEnd = {ActiveCountCallbackRangeEnd};
    SimulationPass* rangeEndPass = new SimulationPass(shgRangeEndComputeShader, rangeEndResources, DispatchCountCallbackRangeEnd, uniformsRangeEnd, passFlag);
    shgPipeline->AddPass(rangeEndPass);

    ComputeShader* shgResetLookUpComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_reset_lookup_table.comp"));
    std::vector<SSBOBinding> ResetLookUpResources = {SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId)};
    DispatchCallback DispatchCountCallbackResetLookUp = [this](){ return glm::ivec3(mHashLookupBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> uniformsResetLookUp = {};
    SimulationPass* ResetLookUpPass = new SimulationPass(shgResetLookUpComputeShader, ResetLookUpResources, DispatchCountCallbackResetLookUp, uniformsResetLookUp, passFlag);
    shgPipeline->AddPass(ResetLookUpPass);

    ComputeShader* shgLookUpComputeShader = new ComputeShader(PathUtil::shader_dir("shg/shg_build_lookup_table.comp"));
    std::vector<SSBOBinding> LookUpResources = {SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId),
                                                SSBOBinding(mUniqueHashBuffer.mBindingLocation, mUniqueHashBuffer.mId),
                                                SSBOBinding(mCellCountBuffer.mBindingLocation, mCellCountBuffer.mId)};
    DispatchCallback DispatchCountCallbackLookUp = [this](){ return glm::ivec3(mCellCountBuffer.ReadBack()[0],1,1); };
    std::vector<UniformCallback> uniformsLookUp = {};
    SimulationPass* LookUpPass = new SimulationPass(shgLookUpComputeShader, LookUpResources, DispatchCountCallbackLookUp, uniformsLookUp, passFlag);
    shgPipeline->AddPass(LookUpPass);

    mPipelines.push_back(shgPipeline);
}

void SimulationManager::CreateSPHPipeline()
{
    SimulationPipeline* sphPipeline = new SimulationPipeline("SPH Pipeline");
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;

    ComputeShader* sphComputeShader = new ComputeShader(PathUtil::shader_dir("sph_particle.comp"));
    std::vector<SSBOBinding> sphResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId),
                                            SSBOBinding(mHashBuffer.mBindingLocation, mHashBuffer.mId),
                                            SSBOBinding(mCellStartBuffer.mBindingLocation, mCellStartBuffer.mId),
                                            SSBOBinding(mCellEndBuffer.mBindingLocation, mCellEndBuffer.mId),
                                            SSBOBinding(mHashLookupBuffer.mBindingLocation, mHashLookupBuffer.mId)};
    DispatchCallback sphDispatchCountCallback = [this](){ return glm::ivec3(mParticleBuffer.mActiveCount,1,1); };
    UniformCallback sphCellSizeCallback = [this](Shader* _shader){ BindCellSizeCallback(_shader); };
    UniformCallback sphSmoothingRadiusCallback = [this](Shader* _shader){ BindSPHSmoothingRadius(_shader); };
    UniformCallback sphNeighborRangeCallback = [this](Shader* _shader){ BindCallback(_shader, "neighborRadius", gNeighborRadius); };
    UniformCallback sphGridMinCallback = [this](Shader* _shader){ BindCallback(_shader, "gridMin", gGridBoundsMin); };
    std::vector<UniformCallback> sphUniforms = {sphCellSizeCallback, sphSmoothingRadiusCallback, sphNeighborRangeCallback, sphGridMinCallback};
    SimulationPass* sphPass = new SimulationPass(sphComputeShader, sphResources, sphDispatchCountCallback, sphUniforms, passFlag);
    sphPipeline->AddPass(sphPass);
    
    mPipelines.push_back(sphPipeline);
}

void SimulationManager::CreateDGPipeline()
{
    SimulationPipeline* dgPipeline = new SimulationPipeline("Dense Grid Pipeline");
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;

    ComputeShader* resetCellRangeComputeShader = new ComputeShader(PathUtil::shader_dir("dg/dg_reset_range.comp"));
    std::vector<SSBOBinding> resetCellRangeResources = {SSBOBinding(mRenderCellStartBuffer.mBindingLocation, mRenderCellStartBuffer.mId),
                                                SSBOBinding(mRenderCellEndBuffer.mBindingLocation, mRenderCellEndBuffer.mId)};
    DispatchCallback resetCellRangeDispatchCountCallback = [this](){ return glm::ivec3(mRenderCellStartBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> resetCellRangeUniforms = {};
    SimulationPass* resetCellRangePass = new SimulationPass(resetCellRangeComputeShader, resetCellRangeResources, resetCellRangeDispatchCountCallback, resetCellRangeUniforms, passFlag);
    dgPipeline->AddPass(resetCellRangePass);

    ComputeShader* gridHashComputeShader = new ComputeShader(PathUtil::shader_dir("dg/dg_grid_hash.comp"));
    std::vector<SSBOBinding> gridHashResources = {SSBOBinding(mParticleBuffer.mBindingLocation, mParticleBuffer.mId),
                                                SSBOBinding(mRenderGridBuffer.mBindingLocation, mRenderGridBuffer.mId)};
    DispatchCallback gridHashDispatchCountCallback = [this](){ return glm::ivec3(mRenderGridBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> gridHashUniforms = {[this](Shader* _shader){ BindCallback(_shader, "cellSize", gCellSize); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "gridMin", gGridBoundsMin); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "gridSize", gGridSize); },
                                                    [this](Shader* _shader){ BindCallback(_shader, "activeCount", int(mRenderGridBuffer.mActiveCount)); }};
    SimulationPass* gridHashPass = new SimulationPass(gridHashComputeShader, gridHashResources, gridHashDispatchCountCallback, gridHashUniforms, passFlag);
    dgPipeline->AddPass(gridHashPass);

    CreateBitonicSortPass(dgPipeline, mRenderGridBuffer);
    CreateFlagPass(dgPipeline, mRenderGridBuffer, mRenderCellFlagBuffer, mRenderCellPrefixBuffer);
    CreateBlellochScanPass(dgPipeline, mRenderCellPrefixBuffer, mRenderCellCountBuffer);

    ComputeShader* cellRangeComputeShader = new ComputeShader(PathUtil::shader_dir("dg/dg_range.comp"));
    std::vector<SSBOBinding> cellRangeResources = {SSBOBinding(mRenderGridBuffer.mBindingLocation, mRenderGridBuffer.mId),
                                                SSBOBinding(mRenderCellStartBuffer.mBindingLocation, mRenderCellStartBuffer.mId),
                                                SSBOBinding(mRenderCellEndBuffer.mBindingLocation, mRenderCellEndBuffer.mId),
                                                SSBOBinding(mRenderCellFlagBuffer.mBindingLocation, mRenderCellFlagBuffer.mId)};
    DispatchCallback cellRangeDispatchCountCallback = [this](){ return glm::ivec3(mRenderGridBuffer.mActiveCount,1,1); };
    std::vector<UniformCallback> cellRangeUniforms = {[this](Shader* _shader){ BindCallback(_shader, "activeCount", int(mRenderGridBuffer.mActiveCount)); }};
    SimulationPass* cellRangePass = new SimulationPass(cellRangeComputeShader, cellRangeResources, cellRangeDispatchCountCallback, cellRangeUniforms, passFlag);
    dgPipeline->AddPass(cellRangePass);

    mPipelines.push_back(dgPipeline);
}

void SimulationManager::CreateBitonicSortPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer)
{
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;
    ComputeShader* sortComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_bitonic_sort.comp"));
    std::vector<SSBOBinding> sortResources = {SSBOBinding(_gridBuffer.mBindingLocation, _gridBuffer.mId)};
    DispatchCallback sortDispatchCountCallback = [this, &_gridBuffer](){ return glm::ivec3(_gridBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> sortUniforms = {};
    ExecuteCallback sortExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BitonicSort(_pass,_count,_groups); };
    SimulationPass* sortPass = new SimulationPass(sortComputeShader, sortResources, sortDispatchCountCallback, sortUniforms, passFlag, sortExecuteCallback);
    _pipeline->AddPass(sortPass);
}

void SimulationManager::CreateFlagPass(SimulationPipeline* _pipeline, GPUBuffer<HashEntry>& _gridBuffer, GPUBuffer<uint32_t>& _flagBuffer, GPUBuffer<uint32_t>& _prefixBuffer)
{
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;
    ComputeShader* computeShader = new ComputeShader(PathUtil::shader_dir("universal/u_hash_entry_flag.comp"));
    std::vector<SSBOBinding> resources = {SSBOBinding(_gridBuffer.mBindingLocation, _gridBuffer.mId),
                                            SSBOBinding(_flagBuffer.mBindingLocation, _flagBuffer.mId),
                                            SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback dispatchCountCallback = [this, &_flagBuffer](){ return glm::ivec3(int(_flagBuffer.mTotalCount), 1, 1); };
    std::vector<UniformCallback> uniforms = {[this, &_flagBuffer](Shader* _shader){ BindCallback(_shader, "activeCount", int(_flagBuffer.mActiveCount)); }};
    SimulationPass* pass = new SimulationPass(computeShader, resources, dispatchCountCallback, uniforms, passFlag);
    _pipeline->AddPass(pass);
}

void SimulationManager::CreateBlellochScanPass(SimulationPipeline* _pipeline, GPUBuffer<uint32_t>& _prefixBuffer, GPUBuffer<uint32_t>& _countBuffer)
{
    unsigned int passFlag = GL_SHADER_STORAGE_BARRIER_BIT;
    ComputeShader* upScanComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_up_scan.comp"));
    std::vector<SSBOBinding> upScanResources = {SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback upScanDispatchCountCallback = [this, &_prefixBuffer](){ return glm::ivec3(_prefixBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> upScanUniforms = {};
    ExecuteCallback upScanExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BlellochScan_Up(_pass,_count,_groups); };
    SimulationPass* upScanPass = new SimulationPass(upScanComputeShader, upScanResources, upScanDispatchCountCallback, upScanUniforms, passFlag, upScanExecuteCallback);
    _pipeline->AddPass(upScanPass);

    ComputeShader* resetComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_reset.comp"));
    std::vector<SSBOBinding> resetResources = {SSBOBinding(_countBuffer.mBindingLocation, _countBuffer.mId),
                                                SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback resetDispatchCountCallback = [this](){ return glm::ivec3(1,1,1); };
    std::vector<UniformCallback> resetUniforms = {};
    SimulationPass* resetPass = new SimulationPass(resetComputeShader, resetResources, resetDispatchCountCallback, resetUniforms, passFlag);
    _pipeline->AddPass(resetPass);

    ComputeShader* downScanComputeShader = new ComputeShader(PathUtil::shader_dir("universal/u_blelloch_down_scan.comp"));
    std::vector<SSBOBinding> downScanResources = {SSBOBinding(_prefixBuffer.mBindingLocation, _prefixBuffer.mId)};
    DispatchCallback downScanDispatchCountCallback = [this, &_prefixBuffer](){ return glm::ivec3(_prefixBuffer.mTotalCount,1,1); };
    std::vector<UniformCallback> downScanUniforms = {};
    ExecuteCallback downScanExecuteCallback = [this](SimulationPass* _pass, glm::ivec3 _count, glm::ivec3 _groups){ BindCustomExecuet_BlellochScan_Down(_pass,_count,_groups); };
    SimulationPass* downScanPass = new SimulationPass(downScanComputeShader, downScanResources, downScanDispatchCountCallback, downScanUniforms, passFlag, downScanExecuteCallback);
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

void SimulationManager::ValidateRenderGrid()
{
    constexpr uint32_t INVALID = 0xFFFFFFFFu;

    // ------------------------------------------------------------
    // Read GPU buffers
    // ------------------------------------------------------------

    auto particles = ReadBuffer(mParticleBuffer);
    auto entries   = ReadBuffer(mRenderGridBuffer);
    auto flags     = ReadBuffer(mRenderCellFlagBuffer);
    auto prefix    = ReadBuffer(mRenderCellPrefixBuffer);
    auto counts    = ReadBuffer(mRenderCellCountBuffer);
    auto starts    = ReadBuffer(mRenderCellStartBuffer);
    auto ends      = ReadBuffer(mRenderCellEndBuffer);

    const uint32_t particleCount = static_cast<uint32_t>(particles.size());
    const uint32_t cellCount     = static_cast<uint32_t>(starts.size());

    LOG_WARNING(
        "Grid buffer sizes: starts={}, ends={}, counts={}, cellCount={}",
        starts.size(),
        ends.size(),
        counts.size(),
        cellCount
    );

    LOG_WARNING("========== RENDER GRID VALIDATION ==========");

    LOG_WARNING(
        "Particles: {}, Entries: {}, Cells: {}",
        particleCount,
        entries.size(),
        cellCount
    );

    // ------------------------------------------------------------
    // 1. Calculate the expected cell for every particle
    // ------------------------------------------------------------

    std::vector<uint32_t> expectedCell(particleCount, INVALID);

    for (uint32_t i = 0; i < particleCount; ++i)
    {
        glm::ivec3 cell = glm::ivec3(glm::floor(
            (particles[i].mPosition - gGridBoundsMin) / gCellSize
        ));

        if (cell.x < 0 ||
            cell.y < 0 ||
            cell.z < 0 ||
            cell.x >= gGridSize.x ||
            cell.y >= gGridSize.y ||
            cell.z >= gGridSize.z)
        {
            expectedCell[i] = INVALID;
            continue;
        }

        uint32_t gridIndex =
            static_cast<uint32_t>(
                cell.x +
                cell.y * gGridSize.x +
                cell.z * gGridSize.x * gGridSize.y
            );

        expectedCell[i] = gridIndex;
    }

    // ------------------------------------------------------------
    // 2. Check sorted entries
    // ------------------------------------------------------------

    LOG_WARNING("--- Checking entries ---");

    bool entriesValid = true;

    for (uint32_t i = 0; i < entries.size(); ++i)
    {
        const HashEntry& entry = entries[i];

        // Ignore unused entries
        if (entry.mHash == INVALID ||
            entry.mIndex == INVALID)
        {
            continue;
        }

        // Particle index must be valid
        if (entry.mIndex >= particleCount)
        {
            LOG_ERROR(
                "ENTRY {} INVALID PARTICLE INDEX: {}",
                i,
                entry.mIndex
            );

            entriesValid = false;
            continue;
        }

        // Compare stored cell against particle's actual cell
        uint32_t actualCell = expectedCell[entry.mIndex];

        if (actualCell != entry.mHash)
        {
            LOG_ERROR(
                "ENTRY {} WRONG CELL: stored cell={}, "
                "particle={}, expected cell={}",
                i,
                entry.mHash,
                entry.mIndex,
                actualCell
            );

            entriesValid = false;
        }

        // Make sure sorting is actually sorted
        if (i > 0)
        {
            const HashEntry& previous = entries[i - 1];

            if (previous.mHash != INVALID &&
                entry.mHash != INVALID &&
                previous.mHash > entry.mHash)
            {
                LOG_ERROR(
                    "SORT ERROR: entry {} hash={} comes after "
                    "entry {} hash={}",
                    i - 1,
                    previous.mHash,
                    i,
                    entry.mHash
                );

                entriesValid = false;
            }
        }
    }

    LOG_WARNING(
        "Entries: {}",
        entriesValid ? "PASS" : "FAIL"
    );

    // ------------------------------------------------------------
    // 3. Check flags
    // ------------------------------------------------------------

    LOG_WARNING("--- Checking flags ---");

    bool flagsValid = true;

    const uint32_t activeCount = particleCount;

    for (uint32_t i = 0; i < activeCount; ++i)
    {
        uint32_t expectedFlag = 0;

        if (i == 0)
        {
            expectedFlag = 1;
        }
        else
        {
            uint32_t currentHash  = entries[i].mHash;
            uint32_t previousHash = entries[i - 1].mHash;

            expectedFlag =
                currentHash != previousHash ? 1 : 0;
        }

        if (flags[i] != expectedFlag)
        {
            LOG_ERROR(
                "FLAG ERROR at {}: GPU={}, expected={}",
                i,
                flags[i],
                expectedFlag
            );

            flagsValid = false;
        }
    }

    LOG_WARNING(
        "Flags: {}",
        flagsValid ? "PASS" : "FAIL"
    );

    // ------------------------------------------------------------
    // 4. Check prefix
    // ------------------------------------------------------------

    LOG_WARNING("--- Checking prefix ---");

    bool prefixValid = true;

    uint32_t running = 0;

    for (uint32_t i = 0; i < activeCount; ++i)
    {
        uint32_t expected = running;

        if (flags[i] == 1)
            running++;

        if (prefix[i] != expected)
        {
            LOG_ERROR(
                "PREFIX ERROR at {}: GPU={}, expected={}",
                i,
                prefix[i],
                expected
            );

            prefixValid = false;
        }
    }

    LOG_WARNING(
        "Prefix: {}",
        prefixValid ? "PASS" : "FAIL"
    );

    // ------------------------------------------------------------
    // 5. Build expected start/end ranges from sorted entries
    // ------------------------------------------------------------

    LOG_WARNING("--- Checking cell ranges ---");

    std::vector<uint32_t> expectedStart(
        cellCount,
        INVALID
    );

    std::vector<uint32_t> expectedEnd(
        cellCount,
        INVALID
    );

    // Entries are sorted by cell.
    // Find contiguous ranges of identical cell indices.

    for (uint32_t i = 0; i < activeCount; ++i)
    {
        if (i >= entries.size())
            break;

        const HashEntry& entry = entries[i];

        if (entry.mHash == INVALID)
            continue;

        uint32_t cell = entry.mHash;

        if (cell >= cellCount)
        {
            LOG_ERROR(
                "ENTRY {} HAS INVALID CELL {}",
                i,
                cell
            );
            continue;
        }

        // First entry belonging to this cell
        if (expectedStart[cell] == INVALID)
        {
            expectedStart[cell] = i;
        }

        // End is one past the final entry.
        expectedEnd[cell] = i + 1;
    }


    // Now compare GPU ranges against CPU ranges.

    bool rangesValid = true;

    for (uint32_t cell = 0; cell < cellCount; ++cell)
    {
        if (starts[cell] != expectedStart[cell])
        {
            LOG_ERROR(
                "CELL START ERROR: cell={}, GPU={}, expected={}",
                cell,
                starts[cell],
                expectedStart[cell]
            );

            rangesValid = false;
        }

        if (ends[cell] != expectedEnd[cell])
        {
            LOG_ERROR(
                "CELL END ERROR: cell={}, GPU={}, expected={}",
                cell,
                ends[cell],
                expectedEnd[cell]
            );

            rangesValid = false;
        }
    }

    LOG_WARNING(
        "Ranges: {}",
        rangesValid ? "PASS" : "FAIL"
    );

    // ------------------------------------------------------------
    // 6. Print compact table of populated cells
    // ------------------------------------------------------------

    LOG_WARNING("--- Populated cells ---");

    for (uint32_t cell = 0; cell < cellCount; ++cell)
    {
        if (expectedStart[cell] == INVALID)
            continue;

        uint32_t count = ends[cell] - starts[cell];

        LOG_DEBUG(
            "Cell {}: start={}, end={}, count={}",
            cell,
            starts[cell],
            ends[cell],
            count
        );

        for (uint32_t i = starts[cell];
             i < ends[cell];
             ++i)
        {
            if (i >= entries.size())
            {
                LOG_ERROR(
                    "    ENTRY {} OUT OF BOUNDS!",
                    i
                );
                continue;
            }

            LOG_DEBUG(
                "    entry {} -> particle {}",
                i,
                entries[i].mIndex
            );
        }
    }

    // ------------------------------------------------------------
    // Final result
    // ------------------------------------------------------------

    bool valid =
        entriesValid &&
        flagsValid &&
        prefixValid &&
        rangesValid;

    LOG_WARNING(
        "========== GRID VALIDATION: {} ==========",
        valid ? "PASS" : "FAIL"
    );
}

bool SimulationManager::ValidateHashGrid()
{
    LOG_WARNING("========== SPATIAL HASH VALIDATION ==========");

    // -------------------------------------------------------------------------
    // Read GPU buffers
    // -------------------------------------------------------------------------

    auto particles = ReadBuffer(mParticleBuffer);
    auto hashes    = ReadBuffer(mHashBuffer);
    auto flags     = ReadBuffer(mCellFlagBuffer);
    auto prefix    = ReadBuffer(mCellPrefixBuffer);
    auto counts    = ReadBuffer(mCellCountBuffer);
    auto starts    = ReadBuffer(mCellStartBuffer);
    auto ends      = ReadBuffer(mCellEndBuffer);
    auto unique    = ReadBuffer(mUniqueHashBuffer);
    auto lookup    = ReadBuffer(mHashLookupBuffer);

    const uint32_t particleCount =
        static_cast<uint32_t>(particles.size());

    const uint32_t hashCount =
        static_cast<uint32_t>(hashes.size());

    const uint32_t flagCount =
        static_cast<uint32_t>(flags.size());

    const uint32_t prefixCount =
        static_cast<uint32_t>(prefix.size());

    const uint32_t uniqueCount =
        static_cast<uint32_t>(unique.size());

    const uint32_t lookupCount =
        static_cast<uint32_t>(lookup.size());

    // This is the number of particles actually processed by your hash shader.
    //
    // Replace this with however you obtain the CPU value if activeCount is
    // different from particleCount.
    const uint32_t activeCount = particleCount;

    LOG_WARNING(
        "Particles={}, HashEntries={}, Flags={}, Prefix={}, "
        "CountBuffer={}, UniqueHashes={}, Lookup={}",
        particleCount,
        hashCount,
        flagCount,
        prefixCount,
        counts.size(),
        uniqueCount,
        lookupCount
    );

    bool valid = true;

    // -------------------------------------------------------------------------
    // GPU hash function -- MUST MATCH THE COMPUTE SHADER
    // -------------------------------------------------------------------------

    auto Hash = [](glm::ivec3 c) -> uint32_t
    {
        constexpr uint32_t h1 = 79801u;
        constexpr uint32_t h2 = 62548u;
        constexpr uint32_t h3 = 32193u;

        return static_cast<uint32_t>(c.x) * h1 ^
               static_cast<uint32_t>(c.y) * h2 ^
               static_cast<uint32_t>(c.z) * h3;
    };

    // -------------------------------------------------------------------------
    // 1. Check particle hashes
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking particle hashes ---");

    std::vector<uint32_t> expectedParticleHashes(particleCount);

    for (uint32_t i = 0; i < particleCount; ++i)
    {
        glm::ivec3 cell =
            glm::ivec3(glm::floor(
                particles[i].mPosition / gCellSize
            ));

        expectedParticleHashes[i] = Hash(cell);
    }

    // -------------------------------------------------------------------------
    // 2. Check hash entries
    //
    // The hash buffer was sorted by hash, so the sorted entries should contain
    // exactly the expected particle/hash pairs, just in sorted order.
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking hash entries ---");

    bool hashesValid = true;

    if (activeCount > hashCount)
    {
        LOG_ERROR(
            "activeCount={} is greater than hash buffer size={}",
            activeCount,
            hashCount
        );

        hashesValid = false;
    }

    // Check every active sorted entry.
    for (uint32_t i = 0; i < activeCount && i < hashCount; ++i)
    {
        const HashEntry& entry = hashes[i];

        if (entry.mIndex >= particleCount)
        {
            LOG_ERROR(
                "HASH ENTRY {} INVALID PARTICLE INDEX: {}",
                i,
                entry.mIndex
            );

            hashesValid = false;
            continue;
        }

        const uint32_t expectedHash =
            expectedParticleHashes[entry.mIndex];

        if (entry.mHash != expectedHash)
        {
            LOG_ERROR(
                "HASH ENTRY {} WRONG HASH: GPU={}, particle={}, expected={}",
                i,
                entry.mHash,
                entry.mIndex,
                expectedHash
            );

            hashesValid = false;
        }

        // Sorted order check.
        if (i > 0 && hashes[i - 1].mHash > entry.mHash)
        {
            LOG_ERROR(
                "HASH SORT ERROR at {}: previous={}, current={}",
                i,
                hashes[i - 1].mHash,
                entry.mHash
            );

            hashesValid = false;
        }
    }

    // -------------------------------------------------------------------------
    // Check inactive entries.
    //
    // Your hash shader explicitly writes:
    //
    // hashes[id].hash          = 0xffffffff;
    // hashes[id].particleIndex = 0xffffffff;
    //
    // for id >= activeCount.
    // -------------------------------------------------------------------------

    for (uint32_t i = activeCount; i < hashCount; ++i)
    {
        if (hashes[i].mHash != 0xffffffffu ||
            hashes[i].mIndex != 0xffffffffu)
        {
            LOG_ERROR(
                "INACTIVE HASH ENTRY {} NOT SENTINEL: "
                "GPU={{hash={}, particle={}}}",
                i,
                hashes[i].mHash,
                hashes[i].mIndex
            );

            hashesValid = false;
        }
    }

    LOG_WARNING(
        "Hash entries: {}",
        hashesValid ? "PASS" : "FAIL"
    );

    valid &= hashesValid;

    // -------------------------------------------------------------------------
    // 3. Check flags
    //
    // A flag is 1 when this entry starts a new hash.
    //
    // flag[0] = 1
    //
    // flag[i] = 1 when:
    //     hashes[i].mHash != hashes[i-1].mHash
    //
    // Otherwise flag = 0.
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking flags ---");

    bool flagsValid = true;

    if (flagCount < activeCount)
    {
        LOG_ERROR(
            "Flag buffer too small: {} < activeCount {}",
            flagCount,
            activeCount
        );

        flagsValid = false;
    }
    else
    {
        for (uint32_t i = 0; i < activeCount; ++i)
        {
            uint32_t expectedFlag = 0;

            if (i == 0)
            {
                expectedFlag = 1;
            }
            else
            {
                expectedFlag =
                    hashes[i].mHash != hashes[i - 1].mHash
                    ? 1
                    : 0;
            }

            if (flags[i] != expectedFlag)
            {
                LOG_ERROR(
                    "FLAG ERROR at {}: GPU={}, expected={}",
                    i,
                    flags[i],
                    expectedFlag
                );

                flagsValid = false;
            }
        }
    }

    LOG_WARNING(
        "Flags: {}",
        flagsValid ? "PASS" : "FAIL"
    );

    valid &= flagsValid;

    // -------------------------------------------------------------------------
    // 4. Check exclusive prefix scan
    //
    // Example:
    //
    // flags:  1 0 1 0 0 1
    // prefix: 0 1 1 2 2 2
    //
    // prefix[i] = number of flags before i.
    //
    // The final number of unique hashes is:
    //
    // prefix[last] + flags[last]
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking prefix ---");

    bool prefixValid = true;

    uint32_t expectedUniqueCount = 0;

    if (activeCount > 0)
    {
        uint32_t running = 0;

        for (uint32_t i = 0; i < activeCount; ++i)
        {
            if (prefix[i] != running)
            {
                LOG_ERROR(
                    "PREFIX ERROR at {}: GPU={}, expected={}",
                    i,
                    prefix[i],
                    running
                );

                prefixValid = false;
            }

            running += flags[i];
        }

        expectedUniqueCount = running;
    }

    LOG_WARNING(
        "Prefix: {} (expected unique hashes={})",
        prefixValid ? "PASS" : "FAIL",
        expectedUniqueCount
    );

    valid &= prefixValid;

    // -------------------------------------------------------------------------
    // 5. Check unique hash buffer
    //
    // Range-start shader does:
    //
    // uint cellIndex = cellScan[id];
    // uniqueHashes[cellIndex] = hashes[id].hash;
    //
    // Therefore each flag==1 entry should populate:
    //
    // uniqueHashes[prefix[i]] = hashes[i].mHash
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking unique hashes ---");

    bool uniqueValid = true;

    if (uniqueCount < expectedUniqueCount)
    {
        LOG_ERROR(
            "Unique hash buffer too small: {} < expected {}",
            uniqueCount,
            expectedUniqueCount
        );

        uniqueValid = false;
    }
    else
    {
        for (uint32_t i = 0; i < activeCount; ++i)
        {
            if (flags[i] != 1)
                continue;

            uint32_t cellIndex = prefix[i];

            if (cellIndex >= uniqueCount)
            {
                LOG_ERROR(
                    "UNIQUE HASH ERROR at entry {}: "
                    "cellIndex={} out of range {}",
                    i,
                    cellIndex,
                    uniqueCount
                );

                uniqueValid = false;
                continue;
            }

            if (unique[cellIndex] != hashes[i].mHash)
            {
                LOG_ERROR(
                    "UNIQUE HASH ERROR at cell {}: GPU={}, expected={}",
                    cellIndex,
                    unique[cellIndex],
                    hashes[i].mHash
                );

                uniqueValid = false;
            }
        }
    }

    LOG_WARNING(
        "Unique hashes: {}",
        uniqueValid ? "PASS" : "FAIL"
    );

    valid &= uniqueValid;

    // -------------------------------------------------------------------------
    // 6. Check cell count
    //
    // Your counter buffer contains ONE uint.
    //
    // The range-end shader uses:
    //
    // layout(std430, binding = 4)
    // buffer CounterBuffer {
    //     uint cellCount;
    // };
    //
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking count buffer ---");

    bool countValid = true;

    if (counts.empty())
    {
        LOG_ERROR("Count buffer is empty");
        countValid = false;
    }
    else
    {
        const uint32_t gpuCellCount = counts[0];

        LOG_WARNING(
            "Unique cell count: GPU={}, expected={}",
            gpuCellCount,
            expectedUniqueCount
        );

        if (gpuCellCount != expectedUniqueCount)
        {
            LOG_ERROR(
                "CELL COUNT ERROR: GPU={}, expected={}",
                gpuCellCount,
                expectedUniqueCount
            );

            countValid = false;
        }
    }

    LOG_WARNING(
        "Count: {}",
        countValid ? "PASS" : "FAIL"
    );

    valid &= countValid;

    // -------------------------------------------------------------------------
    // 7. Check cellStart / cellEnd
    //
    // IMPORTANT:
    //
    // Your range-start shader writes:
    //
    // cellStart[cellIndex] = id;
    //
    // where id is the SORTED HASH ENTRY INDEX.
    //
    // So:
    //
    // cellStart[cell] = first sorted entry
    // cellEnd[cell]   = one-past-last sorted entry
    //
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking cell ranges ---");

    bool rangesValid = true;

    if (starts.size() < expectedUniqueCount)
    {
        LOG_ERROR(
            "Cell start buffer too small: {} < {}",
            starts.size(),
            expectedUniqueCount
        );

        rangesValid = false;
    }

    if (ends.size() < expectedUniqueCount)
    {
        LOG_ERROR(
            "Cell end buffer too small: {} < {}",
            ends.size(),
            expectedUniqueCount
        );

        rangesValid = false;
    }

    if (rangesValid)
    {
        for (uint32_t cell = 0;
             cell < expectedUniqueCount;
             ++cell)
        {
            // Find first sorted entry for this unique hash.
            uint32_t expectedStart = 0;
            uint32_t expectedEnd   = 0;

            bool found = false;

            for (uint32_t i = 0; i < activeCount; ++i)
            {
                if (hashes[i].mHash == unique[cell])
                {
                    if (!found)
                    {
                        expectedStart = i;
                        found = true;
                    }

                    expectedEnd = i + 1;
                }
            }

            if (!found)
            {
                LOG_ERROR(
                    "RANGE ERROR for cell {} / hash {}: "
                    "hash not found in sorted entries",
                    cell,
                    unique[cell]
                );

                rangesValid = false;
                continue;
            }

            if (starts[cell] != expectedStart)
            {
                LOG_ERROR(
                    "CELL START ERROR at cell {}: "
                    "GPU={}, expected={}",
                    cell,
                    starts[cell],
                    expectedStart
                );

                rangesValid = false;
            }

            if (ends[cell] != expectedEnd)
            {
                LOG_ERROR(
                    "CELL END ERROR at cell {}: "
                    "GPU={}, expected={}",
                    cell,
                    ends[cell],
                    expectedEnd
                );

                rangesValid = false;
            }

            if (starts[cell] >= ends[cell])
            {
                LOG_ERROR(
                    "INVALID RANGE at cell {}: start={}, end={}",
                    cell,
                    starts[cell],
                    ends[cell]
                );

                rangesValid = false;
            }

            // Check that every entry in the range has the expected hash.
            for (uint32_t i = starts[cell];
                 i < ends[cell];
                 ++i)
            {
                if (i >= activeCount)
                {
                    LOG_ERROR(
                        "RANGE OUT OF BOUNDS: cell={}, entry={}",
                        cell,
                        i
                    );

                    rangesValid = false;
                    break;
                }

                if (hashes[i].mHash != unique[cell])
                {
                    LOG_ERROR(
                        "RANGE CONTENT ERROR: cell={}, entry={}, "
                        "GPU hash={}, expected={}",
                        cell,
                        i,
                        hashes[i].mHash,
                        unique[cell]
                    );

                    rangesValid = false;
                }
            }
        }
    }

    LOG_WARNING(
        "Cell ranges: {}",
        rangesValid ? "PASS" : "FAIL"
    );

    valid &= rangesValid;

    // -------------------------------------------------------------------------
    // 8. Check hash lookup table
    //
    // This is NOT indexed by cell.
    //
    // The GPU does:
    //
    // uint slot = hash & (lookupSize - 1);
    //
    // and then linear probes.
    //
    // Therefore lookup[cell] is WRONG.
    //
    // We must reproduce the exact lookup algorithm here.
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Checking hash lookup ---");

    bool lookupValid = true;

    if (lookupCount == 0)
    {
        LOG_ERROR("Lookup buffer is empty");
        lookupValid = false;
    }
    else
    {
        // Your shader relies on:
        //
        // slot = hash & (lookupSize - 1)
        //
        // This only works as modulo for a power-of-two table.
        const uint32_t mask = lookupCount - 1;

        if ((lookupCount & mask) != 0)
        {
            LOG_ERROR(
                "LOOKUP ERROR: lookup size {} is not a power of two",
                lookupCount
            );

            lookupValid = false;
        }

        const uint32_t EMPTY_HASH = 0xffffffffu;

        // -------------------------------------------------------------
        // Check every unique hash can be found exactly as the GPU finds it.
        // -------------------------------------------------------------

        for (uint32_t cell = 0;
             cell < expectedUniqueCount;
             ++cell)
        {
            const uint32_t expectedHash = unique[cell];

            uint32_t slot = expectedHash & mask;

            bool found = false;

            for (uint32_t probe = 0;
                 probe < lookupCount;
                 ++probe)
            {
                HashEntry& entry = lookup[slot];

                // Same termination condition as FindCell().
                if (entry.mHash == EMPTY_HASH)
                {
                    LOG_ERROR(
                        "LOOKUP ERROR: hash {} / cell {} "
                        "hit empty slot {} after {} probes",
                        expectedHash,
                        cell,
                        slot,
                        probe
                    );

                    lookupValid = false;
                    break;
                }

                if (entry.mHash == expectedHash)
                {
                    found = true;

                    if (entry.mIndex != cell)
                    {
                        LOG_ERROR(
                            "LOOKUP ERROR at slot {}: "
                            "GPU={{hash={}, cellIndex={}}}, "
                            "expected={{hash={}, cellIndex={}}}",
                            slot,
                            entry.mHash,
                            entry.mIndex,
                            expectedHash,
                            cell
                        );

                        lookupValid = false;
                    }

                    break;
                }

                // Same linear probing as GPU.
                slot = (slot + 1) & mask;
            }

            if (!found)
            {
                LOG_ERROR(
                    "LOOKUP ERROR: hash {} / cell {} "
                    "was not found in lookup table",
                    expectedHash,
                    cell
                );

                lookupValid = false;
            }
        }

        // -------------------------------------------------------------
        // Check every occupied lookup entry.
        //
        // Every occupied entry must point to a valid unique hash.
        // -------------------------------------------------------------

        for (uint32_t slot = 0;
             slot < lookupCount;
             ++slot)
        {
            const HashEntry& entry = lookup[slot];

            if (entry.mHash == EMPTY_HASH)
                continue;

            if (entry.mIndex >= expectedUniqueCount)
            {
                LOG_ERROR(
                    "LOOKUP ERROR at slot {}: "
                    "hash={}, invalid cellIndex={}",
                    slot,
                    entry.mHash,
                    entry.mIndex
                );

                lookupValid = false;
                continue;
            }

            if (unique[entry.mIndex] != entry.mHash)
            {
                LOG_ERROR(
                    "LOOKUP ERROR at slot {}: "
                    "hash={}, cellIndex={}, uniqueHash={}",
                    slot,
                    entry.mHash,
                    entry.mIndex,
                    unique[entry.mIndex]
                );

                lookupValid = false;
            }
        }
    }

    LOG_WARNING(
        "Hash lookup: {}",
        lookupValid ? "PASS" : "FAIL"
    );

    valid &= lookupValid;

    // -------------------------------------------------------------------------
    // 9. Print the resulting spatial hash table
    // -------------------------------------------------------------------------

    LOG_WARNING("--- Spatial hash table ---");

    for (uint32_t cell = 0;
         cell < expectedUniqueCount;
         ++cell)
    {
        const uint32_t hash  = unique[cell];
        const uint32_t start = starts[cell];
        const uint32_t end   = ends[cell];

        LOG_DEBUG(
            "Hash {}: start={}, end={}, count={}",
            hash,
            start,
            end,
            end - start
        );

        for (uint32_t i = start; i < end; ++i)
        {
            if (i >= activeCount)
                break;

            LOG_DEBUG(
                "    entry {} -> particle {}",
                i,
                hashes[i].mIndex
            );
        }
    }

    // -------------------------------------------------------------------------
    // Final result
    // -------------------------------------------------------------------------

    LOG_WARNING(
        "========== SPATIAL HASH VALIDATION: {} ==========",
        valid ? "PASS" : "FAIL"
    );

    return valid;
}