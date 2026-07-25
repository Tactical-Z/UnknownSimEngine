#include "Camera.h"

void Camera::Init()
{

}

const glm::vec3 Camera::GetPosition() const
{
    return mPosition;
}

const glm::vec3 Camera::GetUp() const
{
    return mUp;
}

const glm::vec3 Camera::GetRight() const
{
    return mRight;
}

const glm::vec3 Camera::GetFront() const
{
    return mFront;
}

const float Camera::GetFov() const
{
    return mFOV;
}