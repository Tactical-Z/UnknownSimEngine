#include <Shaders/VisShader.h>
#include <Util/Log.h>

#include <fstream>
#include <sstream>
#include <iostream>

VisShader::VisShader(const std::string& _vertexPath, const std::string& _fragmentPath)
{

    std::string vShaderCodeString = ReadSorceCode(_vertexPath);
    std::string fShaderCodeString = ReadSorceCode(_fragmentPath);

    const char* vShaderCode = vShaderCodeString.c_str();
    const char* fShaderCode = fShaderCodeString.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        LOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {}", infoLog);
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        LOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n {}", infoLog);
    }

    // shader Program
    mId = glCreateProgram();
    glAttachShader(mId, vertex);
    glAttachShader(mId, fragment);
    glLinkProgram(mId);
    // print linking errors if any
    glGetProgramiv(mId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(mId, 512, NULL, infoLog);
        LOG_ERROR("ERROR::SHADER::VIS::PROGRAM::LINKING_FAILED\n {}", infoLog);
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

std::string Shader::ReadSorceCode(const std::string& _filePath)
{
    // Retrieve the shader source code from filePath
    std::string sourceCode;
    std::ifstream sourceFile;

    sourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        sourceFile.open(_filePath);
        std::stringstream sourceStream;
        // read file's buffer contents into streams
        sourceStream << sourceFile.rdbuf();
        // close file handlers
        sourceFile.close();
        // convert stream into string
        sourceCode = sourceStream.str();
    }
    catch (std::ifstream::failure e)
    {
        LOG_ERROR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
        return "";
    }

    return sourceCode.c_str();
}