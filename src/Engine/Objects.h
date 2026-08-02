#pragma once

#include "Util/SMath.h"

class Object
{
public:
    Object() = default;
    Object(glm::vec3 _position);
    virtual ~Object() = default;

protected:
    glm::vec3 mPosition;

public:
    const glm::vec3 GetPosition() const;
};

// black hole radius measured in Schwarzschild radius units
class BlackHole : public Object
{
public:
    BlackHole() = default;
    BlackHole(glm::vec3 _position, float _radius);
    ~BlackHole() = default;
private:

    float mSchwarzschildRadius;
    float mMass;

public:
    const float GetRadius() const;
    const float GetMass() const;
};