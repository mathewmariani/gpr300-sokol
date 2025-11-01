#include "texture.h"

// stb
#include "stb/stb_image.h"

// batteries
#include "batteries/opengl.h"

#include <iostream>

namespace ew {

Texture::Texture(const std::string& path)
{
	int width, height, components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &components, 0);
	if (data == NULL) {
		printf("Failed to load image %s", path.c_str());
		stbi_image_free(data);
		return;
	}

	createTexture(0, data, width, height, components);
	stbi_image_free(data);
}

Texture::Texture(const mipmap_t& mips)
{
	const char *paths[8]{
		mips.mip0,
		mips.mip1,
		mips.mip2,
		mips.mip3,
		mips.mip4,
		mips.mip5,
		mips.mip6,
		mips.mip7,
	};

	for (auto i = 0; i < 8; ++i)
	{
		int width, height, components;
		unsigned char* data = stbi_load(paths[i], &width, &height, &components, 0);
		if (data == NULL) {
			printf("Failed to load image %s", paths[i]);
			stbi_image_free(data);
			return;
		}

		createTexture(i, data, width, height, components);
		stbi_image_free(data);
	}
}

Texture::~Texture()
{
	if (textureID)
	{
		glDeleteTextures(1, &textureID);
	}
}

void Texture::createTexture(int level, const unsigned char *data, int width, int height, int components)
{
	if (textureID == 0)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// sampler
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (data)
	{
		GLint format = (components == 3) ? GL_RGB : GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

}