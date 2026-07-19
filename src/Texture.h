#pragma once

#include <memory> 
#include <string>

using TextureID = unsigned int;

enum class TextureType
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP
};

class Texture
{
public:
    TextureID mID = { 0U }; // Texture id used by glfw
    std::string mPath; // project path to texture location
    TextureType mType;

    Texture() = default;
	Texture(const std::string& _path, TextureType _type = TextureType::TEXTURE_2D);
    Texture(const std::initializer_list<std::string> _paths, TextureType _type = TextureType::TEXTURE_CUBE_MAP);

    // deconstructs object and gl texture reference
    ~Texture();


    // Loads texture into GL using stbi_Image|
    TextureID LoadTexture(const std::string& _path);
    TextureID GenSkybox(std::initializer_list<std::string> _texturePaths);

private:
    // ---------- Local functions --------------



public:

    bool IsValid() const { return mID == 0; }
    TextureID GetTextureID() const { return mID; }
};