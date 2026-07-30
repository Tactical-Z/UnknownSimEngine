#include "Camera.h"

Camera::Camera(glm::vec3 _position)
    :   mPosition(_position)
{

}

Camera::Camera(glm::vec3 _position, float _pich, float _yaw)
    :   mPosition(_position)
{
    SetRotationPY(_pich, _yaw);
}

void Camera::UpdateDirectionVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    front.y = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    front.z = sin(glm::radians(mPitch));

    mFront = glm::normalize(front);

    glm::vec3 worldUp = WORLD_UP;

    mRight = glm::normalize(glm::cross(mFront, worldUp));
    mUp    = glm::normalize(glm::cross(mRight, mFront));
}

void Camera::ZeroRotation()
{
    mPitch = 0;
    mYaw = 0;

    UpdateDirectionVectors();
}

void Camera::SetRotationPY(float _pitch, float _yaw)
{
    mPitch = _pitch;
    mYaw = _yaw;

    mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

    UpdateDirectionVectors();
}

void Camera::SetRotationP(float _pitch)
{
    mPitch = _pitch;
    mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

    UpdateDirectionVectors();
}

void Camera::SetRotationY(float _yaw)
{
    mYaw = _yaw;

    UpdateDirectionVectors();
}

void Camera::AddRotationPY(float _pitch, float _yaw)
{
    mPitch += _pitch;
    mYaw += _yaw;

    mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

    UpdateDirectionVectors();
}

void Camera::AddRotationP(float _pitch)
{
    mPitch += _pitch;
    mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

    UpdateDirectionVectors();
}

void Camera::AddRotationY(float _yaw)
{
    mYaw += _yaw;

    UpdateDirectionVectors();
}

void Camera::SetPosition(const glm::vec3 _newPos)
{
    mPosition = _newPos;
}

void Camera::AddPosition(const glm::vec3 _newPos)
{
    mPosition += _newPos;
}

const glm::vec3 Camera::GetPosition() const
{
    return mPosition;
}

const float Camera::GetPitch() const
{
    return mPitch;
}

const float Camera::GetYaw() const
{
    return mYaw;
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

const float Camera::GetFarplane() const 
{
    return mFarPlane;
}

const int Camera::GetNumRayChecks() const 
{
    return mNumRayChecks;
}