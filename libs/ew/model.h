/*
*	Author: Eric Winebrenner
*/

#pragma once

#include "mesh.h"

// sokol
#include "sokol/sokol_fetch.h"

#include <string>
#include <vector>

namespace ew {

class Model {
public:
	explicit Model(const std::string &path);
	~Model() = default;

	void draw();

public:
	std::vector<ew::Mesh> m_meshes;

private:
	void createModel(const std::vector<ew::MeshData>& data);
	static void fetchCallback(const sfetch_response_t* response);
};

}