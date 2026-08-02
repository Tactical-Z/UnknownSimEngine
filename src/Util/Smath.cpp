#include "Util/SMath.h"

std::random_device SMath::randomDevice;
std::mt19937 SMath::gen(randomDevice());

glm::vec3 SMath::RandomVecByBounds(const glm::vec3& _min, const glm::vec3& _max)
{
    std::uniform_real_distribution<float> distribX(_min.x, _max.x);
    std::uniform_real_distribution<float> distribY(_min.y, _max.y);
    std::uniform_real_distribution<float> distribZ(_min.z, _max.z);

    return glm::vec3(distribX(gen), distribY(gen), distribZ(gen));
}

float SMath::RandomFloatByBounds(const float& _min, const float& _max)
{
    std::uniform_real_distribution<float> distrib(_min, _max);
    return distrib(gen);
}

float SMath::RandomFloatByBounds(const glm::vec2& _bounds)
{
    std::uniform_real_distribution<float> distrib(_bounds.x, _bounds.y);
    return distrib(gen);
}

float SMath::MassFromSchwarzschildRadius(const float& _sRadius)
{
    // Schwarzschild radius equiation reordered for mass
    return ((_sRadius * (C * C)) / (2 * G));
}

float SMath::OrbitalVelocity(const float& _orbitRadius, const float& _orbitBodyMass)
{
    return sqrt((G * _orbitBodyMass) / _orbitRadius);
}

float SMath::RadiusBetweenTwoPoints(const glm::vec3& _p1, const glm::vec3& _p2)
{
    return glm::distance(_p1, _p2);
}