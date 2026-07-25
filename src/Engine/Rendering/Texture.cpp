
#include "Texture.h"
#include "Util/Log.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(unsigned int _width, unsigned int _height, TextureType _type) : mPath(""), mType(_type)
{	
    mID = MakeBlankTexture(_width, _height);
}

Texture::Texture(const std::string& _path, TextureType _type) : mPath(_path), mType(_type)
{	
    mID = LoadTexture(_path);
}

Texture::Texture(const std::initializer_list<std::string> _paths, TextureType _type) : mType(_type)
{
    mID = GenSkybox(_paths);
}

Texture::~Texture()
{
	// deletes texture from gl memory
	glDeleteTextures(1,&mID);
}

void Texture::use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mID);
	
}

void Texture::useCustomTex()
{
	glBindImageTexture(0, mID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
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
		LOG_ERROR("Texture Load Failed: %s", _path.c_str());
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

TextureID Texture::GenSkybox(std::initializer_list<std::string> _texturePaths)
{
	TextureID texID;
	// init all textures and assign them to the correct channels
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	stbi_set_flip_vertically_on_load(false);

	int width, height, nrChannels, index{ 0 };
	for (auto texturePath : _texturePaths)
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
			LOG_ERROR("Cubemap texture failed to load at location: %s", texturePath.c_str());
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