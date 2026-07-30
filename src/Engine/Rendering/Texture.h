#pragma once

#include <memory> 
#include <string>
#include <vector>

using TextureID = unsigned int;

enum class TextureType
{
    TT_IMAGE2D,
    TT_SAMPLER2D,
    TT_SAMPLERCUBE
};

class Texture
{
public:
    TextureID mID = { 0U }; // Texture id used by glfw
    std::string mPath; // project path to texture location

    Texture() = default;
    Texture(unsigned int _width, unsigned int _height);
	Texture(const std::string& _path);
    Texture(const std::vector<std::string>& _paths);

    // deconstructs object and gl texture reference
    ~Texture();

    void Bind(int _location, TextureType _texType);

private:
    // ---------- Local functions --------------

    // Loads texture into GL using stbi_Image|
    TextureID LoadTexture(const std::string& _path);
    TextureID GenSkybox(const std::vector<std::string>& _texturePaths);
    TextureID MakeBlankTexture(unsigned int _width, unsigned int _height);

public:

    bool IsValid() const { return mID == 0; }
    TextureID GetTextureID() const { return mID; }
};