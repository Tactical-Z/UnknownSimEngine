#pragma once

#include <functional>
#include <vector>
#include <glad/glad.h>

using UniformCallback = std::function<void(class Shader*)>;
using DispatchCallback = std::function<uint32_t()>;
using ExecuteCallback = std::function<void(class SimulationPass*, uint32_t, uint32_t)>; // for custom execution code

enum class BindingLocation{
    BL_PARTICLE_BUFFER,
    BL_SHG_HASH_BUFFER,
    BL_SHG_START_BUFFER,
    BL_SHG_END_BUFFER,
    BL_SHG_CELL_COUNT_BUFFER,
    BL_SHG_UNIQUE_HASH_BUFFER,
    BL_SHG_CELL_FLAG_BUFFER,
    BL_SHG_CELL_PREFIX_BUFFER,
    BL_SHG_HASH_LOOKUP_BUFFER
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