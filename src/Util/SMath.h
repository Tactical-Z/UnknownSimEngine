#pragma once

#include <random>
#include <numbers>
#include "glm/glm.hpp"

#define PI std::numbers::pi
#define TWO_PI PI * 2
#define WORLD_UP glm::vec3(0.0f, 0.0f, 1.0f);

// Using geometric scaling with constant values to save the program caulating 
// high floating point accuracy numbers which can quickly generate small
//  errors over time, while still maintaining physical accuracy.

// Length unit
#define LIGHT_SPEED 1
#define C LIGHT_SPEED
// Mass Unit
#define GRAV_CONSTANT 1
#define G GRAV_CONSTANT
// Time Unit
#define SCHWARZSCHILD_TIME 1
#define S SCHWARZSCHILD_TIME

class SMath
{
public:
    SMath() = default;
    ~SMath() = default;

    static glm::vec3 RandomVecByBounds(const glm::vec3& _min, const glm::vec3& _max);
    static float RandomFloatByBounds(const float& _min, const float& _max);
    static float RandomFloatByBounds(const glm::vec2& _bounds);
    static float MassFromSchwarzschildRadius(const float& _sRadius);
    // Radius in Schwarzschild as unit, return in fraction of speed of light.
    static float OrbitalVelocity(const float& _orbitRadius, const float& _orbitBodyMass);
    // Return in Schwarzschild radius unit
    static float RadiusBetweenTwoPoints(const glm::vec3& _p1, const glm::vec3& _p2);
    private:
    static std::random_device randomDevice;
    static std::mt19937 gen;


};