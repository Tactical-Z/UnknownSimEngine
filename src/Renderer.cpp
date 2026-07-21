#include "Renderer.h"
#include "PathUtil.h"
#include "Log.h"
#include "TimeManager.h"

#include <glad/glad.h>

Renderer::~Renderer(){
    delete mDisplayTexture;
    delete mVisShader;
    delete mComputeShader;
}

void Renderer::Init()
{
    InitBuffers();
    InitShaders();
    InitTextures();
}

void Renderer::Render()
{
    // Compute Shader
    mComputeShader->use();
    mComputeShader->setFloat("time", TimeManager::GetTimeSinceAppStart());
    mDisplayTexture->useCustomTex();
    glDispatchCompute((unsigned int)512, (unsigned int)512, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    // Render Shader
    glBindVertexArray(mVAO);
    mVisShader->use();
    mDisplayTexture->use();
    mVisShader->setInt("textureSampler", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::InitBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
}

void Renderer::InitShaders()
{
    mVisShader = new VisShader(PathUtil::shader_dir("general.vert"), PathUtil::shader_dir("general.frag"));
    mComputeShader = new ComputeShader(PathUtil::shader_dir("general.comp"));
}

void Renderer::InitTextures()
{
   mDisplayTexture = new Texture(512,512);
}