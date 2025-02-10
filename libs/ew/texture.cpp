#include "texture.h"

// stb
#include "stb/stb_image.h"

// sokol
#include "sokol/sokol_fetch.h"

// opengl
#include <GLES3/gl3.h>

#include <iostream>

namespace {

static uint8_t file_buffer[1024 * 1024 * 10];

struct fetch_wrapper
{
	void *ptr;
};

struct mipmap_request_instance_t
{
	int index;
	void *ptr;
};

}

namespace ew {

Texture::Texture(const std::string& path)
{
	fetch_wrapper w = {
		.ptr = this,
	};

	sfetch_send((sfetch_request_t){
		.path = path.c_str(),
		.callback = fetchCallback,
		.buffer = {
			.ptr =  malloc(1024 * 1024 * 10),
			.size = 1024 * 1024 * 10,
		},
		.user_data = SFETCH_RANGE(w),
	});
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
		mipmap_request_instance_t instance = {
			.index = i,
			.ptr = this,
		};
		if (paths[i] == nullptr)
		{
			continue;
		}
		sfetch_send((sfetch_request_t){
			.path = paths[i],
			.callback = fetchMipMapCallback,
			.buffer = SFETCH_RANGE(file_buffer),
			.user_data = SFETCH_RANGE(instance),
		});
	}
}

Texture::~Texture()
{
	if (textureID)
	{
		glDeleteTextures(1, &textureID);
	}
}

void Texture::createTexture(int level, const unsigned char *data, int width, int height)
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
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::fetchCallback(const sfetch_response_t* response)
{
	if (!response->fetched)
	{
		printf("[!!!] Failed to load texture (%s)\n", response->path);
		return;
	}

	int width, height, components;
	auto *pixels = stbi_load_from_memory((const stbi_uc *)response->data.ptr, response->data.size, &width, &height, &components, 4);

	auto* self = (Texture*)(static_cast<fetch_wrapper*>(response->user_data))->ptr;

	if (pixels)
	{
		self->createTexture(0, pixels, width, height);
	}

	stbi_image_free(pixels);
}

void Texture::fetchMipMapCallback(const sfetch_response_t* response)
{
	if (!response->fetched)
	{
		printf("[!!!] Failed to load texture (%s)\n", response->path);
		return;
	}

	int width, height, components;
	auto *pixels = stbi_load_from_memory((const stbi_uc *)response->data.ptr, response->data.size, &width, &height, &components, 4);

	auto instance = static_cast<mipmap_request_instance_t*>(response->user_data);
	auto* self = static_cast<Texture*>(instance->ptr);

	if (pixels)
	{
		self->createTexture(instance->index, pixels, width, height);
	}

	stbi_image_free(pixels);
}

}