#include "shaders/Shader.h"
#include "core/errorHandling/Log.h"

Shader::Shader(Logger* _logger)
    :   mLogger(_logger)
{

}

void Shader::use() const
{
    glUseProgram(mId);
}

std::string Shader::GetSrc()
{
    return mSrc;
}