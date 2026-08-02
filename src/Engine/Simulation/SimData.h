#pragma once

#include <functional>
#include <glad/glad.h>

using UniformCallback = std::function<void(class Shader*)>;

enum class BindingLocation{
    BL_PARTICLE_BUFFER, 
};

struct SSBOBinding{
    BindingLocation mLocation;
    GLint mBufferID;
};