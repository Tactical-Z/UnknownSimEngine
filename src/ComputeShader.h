#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:

    unsigned int mId;

    ComputeShader() = default;
    ComputeShader(const std::string& _computePath);
    ~ComputeShader() = default;

private:

public:

};