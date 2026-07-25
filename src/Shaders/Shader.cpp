#include "Shader.h"

void Shader::use() const
{
    glUseProgram(mId);
}