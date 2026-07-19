#include "Renderer.h"
#include "PathUtil.h"
#include <glad/glad.h>
#include "Log.h"

Renderer::~Renderer(){
    delete mDisplayTexture;
    delete mGeneralShader;
}

void Renderer::Init()
{
    InitBuffers();
    InitShaders();
    InitTextures();
}

void Renderer::Render()
{
    glBindVertexArray(mVAO);
    mGeneralShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mDisplayTexture->mID);
    mGeneralShader->setInt("textureSampler", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::InitBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
}

void Renderer::InitShaders()
{
    mGeneralShader = new Shader(PathUtil::shader_dir("general.vert"), PathUtil::shader_dir("general.frag"));
}

void Renderer::InitTextures()
{
    mDisplayTexture = new Texture(PathUtil::asset_dir("placeholderimage.jpg"));
}