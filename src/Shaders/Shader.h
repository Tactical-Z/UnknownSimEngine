#pragma once 

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int mId;
    std::string mSrc = "";

    Shader() = default;
    ~Shader() = default;

    // Setts this shader to be used for gl context.
    void use() const;

    std::string GetSrc();
protected:

    std::string ReadSorceCode(const std::string& _filePath);

public:

    void setBool(const std::string& _name, bool _value) const
    {
        glUniform1i(glGetUniformLocation(mId, _name.c_str()), (int)_value);
    }

    void setInt(const std::string& _name, int _value) const
    {
        glUniform1i(glGetUniformLocation(mId, _name.c_str()), _value);
    }

    void setUInt(const std::string& _name, int _value) const
    {
        glUniform1ui(glGetUniformLocation(mId, _name.c_str()), _value);
    }

    void setFloat(const std::string& _name, float _value) const
    {
        glUniform1f(glGetUniformLocation(mId, _name.c_str()), _value);
    }

    void setVec2(const std::string& _name, const glm::vec2& _value) const
    {
        glUniform2fv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setIVec2(const std::string& _name, const glm::ivec2& _value) const
    {
        glUniform2iv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setVec3(const std::string& _name, const glm::vec3& _value) const
    {
        glUniform3fv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setIVec3(const std::string& _name, const glm::ivec3& _value) const
    {
        glUniform3iv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setVec4(const std::string& _name, const glm::vec4& _value) const
    {
        glUniform4fv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setIVec4(const std::string& _name, const glm::ivec4& _value) const
    {
        glUniform4iv(glGetUniformLocation(mId, _name.c_str()), 1, &_value[0]);
    }

    void setMat2(const std::string& _name, const glm::mat2& _mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(mId, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }

    void setMat3(const std::string& _name, const glm::mat3& _mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(mId, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }

    void setMat4(const std::string& _name, const glm::mat4& _mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(mId, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }

};