#include "texture.h"

// stb
#include "stb/stb_image.h"

// sokol
#include "sokol/sokol_fetch.h"

// opengl
#include <GLES3/gl3.h>

#include <iostream>

namespace {
	struct fetch_wrapper {
		void *ptr;
	};
}

namespace ew {

Texture::Texture(const std::string& path) {

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

Texture::~Texture() {
	if (textureID) {
		glDeleteTextures(1, &textureID);
	}
}

void Texture::createTexture(const unsigned char *data, int width, int height)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	loaded = true;
}

void Texture::fetchCallback(const sfetch_response_t* response) {
	if (!response->fetched) {
		printf("[!!!] Failed to load texture (%s)\n", response->path);
		return;
	}

	auto* self = (Texture*)(static_cast<fetch_wrapper*>(response->user_data))->ptr;
	printf("Scene.texture at: %p\n", (void*)self);

	int width, height, components;
	auto *pixels = stbi_load_from_memory((const stbi_uc *)response->data.ptr, response->data.size, &width, &height, &components, 4);

	if (pixels) {
		self->createTexture(pixels, width, height);
	}

	stbi_image_free(pixels);
}

}