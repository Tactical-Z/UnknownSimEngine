#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:

    ComputeShader() = default;
    ComputeShader(const std::string& _computePath);
    ~ComputeShader() = default;

private:

public:

};