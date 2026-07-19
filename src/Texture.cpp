
#include <Texture.h>
#include <Log.h>
#include <glad/glad.h>
#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

Texture::Texture(const std::string& _path, TextureType _type) : mPath(_path), mType(_type)
{	
    mID = LoadTexture(_path);
}

Texture::Texture(const std::initializer_list<std::string> _paths, TextureType _type = TextureType::TEXTURE_CUBE_MAP) : mType(_type)
{
    mID = GenSkybox(_paths);
}

Texture::~Texture()
{
	// deletes texture from gl memory
	glDeleteTextures(1,&mID);
}

TextureID Texture::LoadTexture(const std::string& _path)
{
    TextureID texID;

	// gl generate textures
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Set texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, texID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		LOG_ERROR("Texture Load Failed: %s", _path.c_str());
	}
	stbi_image_free(data);
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
	return texID;
}