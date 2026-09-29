#pragma once
#include "shaders/Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader() = default;
    ComputeShader(const std::string& _computePath, class Logger* _logger);
    ~ComputeShader() = default;
};