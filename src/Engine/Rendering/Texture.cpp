
#include "Texture.h"
#include "Util/Log.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(unsigned int _width, unsigned int _height)
	:	mPath("")
{	
    mID = MakeBlankTexture(_width, _height);
}

Texture::Texture(const std::string& _path)
	:	mPath(_path)
{	
    mID = LoadTexture(_path);
}

Texture::Texture(const std::vector<std::string>& _paths)
{
    mID = GenSkybox(_paths);
}

Texture::~Texture()
{
	// deletes texture from gl memory
	glDeleteTextures(1,&mID);
}

// ToDo: change from use to bind or something like that
void Texture::Bind(int _location, TextureType _texType)
{
	switch (_texType)
	{
	case TextureType::TT_IMAGE2D:
		glBindImageTexture(_location, mID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		break;
	case TextureType::TT_SAMPLER2D:
		glActiveTexture(GL_TEXTURE0 + _location);
		glBindTexture(GL_TEXTURE_2D, mID);
		break;
	case TextureType::TT_SAMPLERCUBE:
		glActiveTexture(GL_TEXTURE0 + _location);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
		break;
	default:
		break;
	}
}

TextureID Texture::LoadTexture(const std::string& _path)
{
    TextureID texID;

	// gl generate textures
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Set texture wrapping/filtering options
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// stbi init textures
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(_path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		LOG_ERROR("Texture Load Failed: {}", _path.c_str());
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

TextureID Texture::GenSkybox(const std::vector<std::string>& _texturePaths)
{
	TextureID texID;
	// init all textures and assign them to the correct channels
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	stbi_set_flip_vertically_on_load(false);

	int width, height, nrChannels, index{ 0 };
	for (std::string texturePath : _texturePaths)
	{
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index++,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			LOG_ERROR("Cubemap texture failed to load at location: {}", texturePath.c_str());
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texID;
}

TextureID Texture::MakeBlankTexture(unsigned int _width, unsigned int _height){
	unsigned int texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, _width, _height);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}