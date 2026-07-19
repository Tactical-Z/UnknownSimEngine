#pragma once

#include "Shader.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    Shader mGeneralShader;

    void Init();
    void Render();

private:

    void InitShaders();

public:


};