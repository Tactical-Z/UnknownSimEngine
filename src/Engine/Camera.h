#pragma once

#include "Util/SMath.h"

class Camera{
public:
    Camera() = default;
    Camera(glm::vec3 _position);
    Camera(glm::vec3 _position, float _pich, float _yaw);
    ~Camera() = default;

private:

    glm::vec3 mPosition = glm::vec3(-40.0f, 0.0f, 0.0f);
    float mPitch = 0;
    float mYaw = 0;
    glm::vec3 mUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 mRight = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 mFront = glm::vec3(1.0f, 0.0f, 0.0f);
    float mFOV = 45.0f;
    float mFarPlane = 100.f;
    int mNumRayChecks = 200;

    void UpdateDirectionVectors();
public:

    void ZeroRotation();
    void SetRotationPY(float _pitch, float _yaw);
    void SetRotationP(float _pitch);
    void SetRotationY(float _yaw);
    void AddRotationPY(float _pitch, float _yaw);
    void AddRotationP(float _pitch);
    void AddRotationY(float _yaw);
    void SetPosition(const glm::vec3 _newPos);
    void AddPosition(const glm::vec3 _newPos);
    
    const glm::vec3 GetPosition() const;
    const float GetPitch() const;
    const float GetYaw() const;
    const glm::vec3 GetUp() const;
    const glm::vec3 GetRight() const;
    const glm::vec3 GetFront() const;
    const float GetFov() const;
    const float GetFarplane() const;
    const int GetNumRayChecks() const;
};

