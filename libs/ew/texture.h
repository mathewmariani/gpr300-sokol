#pragma once

// sokol
#include "sokol/sokol_fetch.h"

#include <string>

namespace ew {

class Texture {
public:
	explicit Texture(const std::string& url);
	~Texture();

	bool isLoaded() const { return loaded; }
	unsigned int getID() const { return textureID; }

private:
	unsigned int textureID = 0;
	bool loaded = false;

	void createTexture(const unsigned char *data, int width, int height);
	static void fetchCallback(const sfetch_response_t* response);
};

}
