#pragma once

#include <glm/glm.hpp>

class Camera{
public:
    Camera() = default;
    ~Camera() = default;

    void Init();
      
private:

    glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 mUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 mRight = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 mFront = glm::vec3(1.0f, 0.0f, 0.0f);
    float mFOV = 45.0f;

public:
    const glm::vec3 GetPosition() const;
    const glm::vec3 GetUp() const;
    const glm::vec3 GetRight() const;
    const glm::vec3 GetFront() const;
    const float GetFov() const;
};

