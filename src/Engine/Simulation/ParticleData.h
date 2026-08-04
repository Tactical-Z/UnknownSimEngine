#pragma once

#include "glm/glm.hpp"

#define NUM_PARTICLE_MAX 20
struct Particle{
    glm::vec3 mPosition;
    float mRadius;
    glm::vec3 mVelocity;
    float mMass;
};