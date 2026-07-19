#include "Renderer.h"

#include <glad/glad.h>

void Renderer::Init()
{
    InitShaders();
}

void Renderer::InitShaders()
{
    mGeneralShader = Shader("shader_src/general.vert", "shader_src/general.frag");
}

void Renderer::Render()
{
    mGeneralShader.use();
    //glBindTexture(GL_TEXTURE_2D, outputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
