#version 450 core

in vec2 uv;

layout(location = 0) out vec4 color;

uniform sampler2D textureSampler;

void main()
{
    color = texture(textureSampler, uv);
}