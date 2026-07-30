#pragma once

#include <random>
#include <numbers>
#include "glm/glm.hpp"

#define PI std::numbers::pi
#define TWO_PI PI * 2
#define WORLD_UP glm::vec3(0.0f, 0.0f, 1.0f);
class SMath
{
public:
    SMath() = default;
    ~SMath() = default;

    static glm::vec3 RandomVecByBounds(const glm::vec3& _min, const glm::vec3& _max);
    static float RandomFloatByBounds(const float& _min, const float& _max);
    static float RandomFloatByBounds(const glm::vec2& _bounds);
private:
    static std::random_device randomDevice;
    static std::mt19937 gen;


};