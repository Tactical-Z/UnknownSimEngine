#include "Shaders/ComputeShader.h"
#include "Util/Log.h"
#include <glad/glad.h>


ComputeShader::ComputeShader(const std::string& _computePath)
{

    std::string sourceCodeString = ReadSorceCode(_computePath);
    const char* sourceCode = sourceCodeString.c_str();

    unsigned int compute;
    int success;
    char infoLog[512];

    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &sourceCode, nullptr);
    glCompileShader(compute);
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        LOG_ERROR("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n {}", infoLog);
    }

    mId = glCreateProgram();
    glAttachShader(mId, compute);
    glLinkProgram(mId);
    glGetProgramiv(mId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(mId, 512, NULL, infoLog);
        LOG_ERROR("ERROR::SHADER::COMPUTE::PROGRAM::LINKING_FAILED\n {}", infoLog);
    }

    glDeleteShader(compute);
}

