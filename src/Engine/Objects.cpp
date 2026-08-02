#include "Engine/Objects.h"
#include "Util/Log.h"

Object::Object(glm::vec3 _position)
: mPosition(_position)
{

}

const glm::vec3 Object::GetPosition() const
{
    return mPosition;
}

BlackHole::BlackHole(glm::vec3 _position, float _radius)
: Object(_position), mSchwarzschildRadius(_radius)
{
    mMass = SMath::MassFromSchwarzschildRadius(_radius);
}

const float BlackHole::GetRadius() const
{
    return mSchwarzschildRadius;
}

const float BlackHole::GetMass() const
{
    return mMass;
}