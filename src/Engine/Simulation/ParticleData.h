#pragma once

#include "glm/glm.hpp"

#define NUM_PARTICLE_MAX 100
constexpr float gParticleRadius = 0.5f;
struct Particle{
    glm::vec3 mPosition;
    float mRadius;
    glm::vec3 mVelocity;
    float mMass;
    glm::vec4 mColor;
};