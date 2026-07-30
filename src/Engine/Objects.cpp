#include "Engine/Objects.h"

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

}

const float BlackHole::GetRadius() const
{
    return mSchwarzschildRadius;
}