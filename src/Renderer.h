#pragma once

#include "Shader.h"
#include "Texture.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    unsigned int mVAO;
    Texture* mDisplayTexture = nullptr;
    Shader* mGeneralShader = nullptr;

    void Init();
    void Render();

private:

    void InitBuffers();
    void InitShaders();
    void InitTextures();

public:


};