#pragma once

#include <functional>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>


// Particle Data
// -----------------------------
// cellSize >= smoothingRadius
#define NUM_PARTICLE_MAX 10000
constexpr float gParticleRadius = 0.5f;
constexpr float gSmoothingRadius = 2.5f;

struct Particle{
    glm::vec3 mPosition;
    float mRadius;
    glm::vec3 mVelocity;
    float mMass;
    glm::vec4 mColor;
};

// Grid Data
// -----------------------------
constexpr glm::vec3 gGridBoundsMin = glm::vec3(-25);
constexpr glm::vec3 gGridBoundsMax = glm::vec3(25);
constexpr float gCellSize = 5.0f;
const glm::ivec3 gGridSize = glm::ivec3(glm::ceil((gGridBoundsMax - gGridBoundsMin) / gCellSize));
const int gCellCount = gGridSize.x * gGridSize.y * gGridSize.z;
const int gNeighborRadius = int(ceil(gParticleRadius / gCellSize));

struct HashEntry{
    unsigned int mHash;
    unsigned int mIndex;
};

// Simulation Data
// -----------------------------
using UniformCallback = std::function<void(class Shader*)>;
using DispatchCallback = std::function<glm::ivec3()>;
using ExecuteCallback = std::function<void(class SimulationPass*, glm::ivec3, glm::ivec3)>; // for custom execution code

// Todo: make global buffers, like 1 flag 1 hash 1 start and so on
enum class BindingLocation{
    BL_PARTICLE_BUFFER = 0,
    BL_HASH_ENTRY_BUFFER = 1,
    BL_RANGE_START_BUFFER = 2,
    BL_RANGE_END_BUFFER = 3,
    BL_COUNT_BUFFER = 4,
    BL_SHG_UNIQUE_HASH_BUFFER = 5,
    BL_FLAG_BUFFER = 6,
    BL_PREFIX_BUFFER = 7,
    BL_SHG_HASH_LOOKUP_BUFFER = 8,
};

struct SSBOBinding{
    BindingLocation mLocation;
    GLint mBufferID;
};

template <typename T>
struct GPUBuffer {
    GLuint mId;
    BindingLocation mBindingLocation;
    uint32_t mActiveCount;
    uint32_t mTotalCount;
    std::vector<T> mBufferData;
    
    bool HasCPUData() const
    {
        return !mBufferData.empty();
    }

    size_t GetSizeBytes() const
    {
        return mBufferData.size() * sizeof(T);
    }

    std::vector<T> ReadBack()
    {
        std::vector<T> data(mTotalCount);

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            mId
        );

        glGetBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            0,
            GetSizeBytes(),
            data.data()
        );

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            0
        );

        return data;
    }

    void Generate()
    {
        glGenBuffers(1, &mId);

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            mId
        );

        if(HasCPUData())
        {
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                GetSizeBytes(),
                mBufferData.data(),
                GL_DYNAMIC_DRAW
            );
        }
        else
        {
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                GetSizeBytes(),
                nullptr,
                GL_DYNAMIC_DRAW
            );
        }

        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            0
        );
    }
};