#pragma once
#include "shaders/Shader.h"

class VisShader : public Shader
{
public:
    VisShader() = default;
    VisShader(const std::string& _vertexPath, const std::string& _fragmentPath, class Logger* _logger);
    ~VisShader() = default;
};