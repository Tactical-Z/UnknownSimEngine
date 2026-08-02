#include "Shader.h"

void Shader::use() const
{
    glUseProgram(mId);
}

std::string Shader::GetSrc()
{
    return mSrc;
}