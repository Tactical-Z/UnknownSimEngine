#pragma once

#include <functional>
#include <vector>
#include <string>
#include "glm/glm.hpp"

using CallbackVoidNull = std::function<void()>;
using CallbackVoidFloat = std::function<void(float)>;
using CallbackVoidInt = std::function<void(int)>;
using CallbackFloatNull = std::function<float()>;
using CallbackFloatRefNull = std::function<float&()>;
using CallbackIntNull = std::function<int()>;
using CallbackCCharVecNull = std::function<const std::vector<const char*>&()>;

using CallbackCameraRefNull = std::function<class Camera*()>;
using CallbackVecPairCCharFloatNull = std::function<std::vector<std::pair<const char*, float>>()>;
using CallbackiVec2Null = std::function<glm::ivec2()>;