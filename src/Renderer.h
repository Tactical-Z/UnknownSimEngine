#pragma once

#include "VisShader.h"
#include "ComputeShader.h"
#include "Texture.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    unsigned int mVAO;
    Texture* mDisplayTexture = nullptr;
    VisShader* mVisShader = nullptr;
    ComputeShader* mComputeShader = nullptr;
    void Init();
    void Render();

private:

    void InitBuffers();
    void InitShaders();
    void InitTextures();

public:


};