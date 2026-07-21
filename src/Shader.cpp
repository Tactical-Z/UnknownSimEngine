#include "Shader.h"

#include <Log.h>

void Shader::use() const
{
    glUseProgram(mId);
}