#pragma once

// sokol
#include "sokol/sokol_fetch.h"

#include <string>

namespace ew {

struct mipmap_t
{
	const char *mip0;
	const char *mip1;
	const char *mip2;
	const char *mip3;
	const char *mip4;
	const char *mip5;
	const char *mip6;
	const char *mip7;
};

class Texture {
public:
	Texture(const std::string& url);
	Texture(const mipmap_t& paths);
	~Texture();

	bool isLoaded() const { return loaded; }
	unsigned int getID() const { return textureID; }

private:
	unsigned int textureID = 0;
	bool loaded = false;

	void createTexture(int level, const unsigned char *data, int width, int height);
	static void fetchCallback(const sfetch_response_t* response);
	static void fetchMipMapCallback(const sfetch_response_t* response);
};

}
