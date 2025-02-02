/*
*	Author: Eric Winebrenner
*/

#pragma once
#include "mesh.h"
#include "shader.h"
#include <vector>

namespace ew {
	class Model {
	public:
		static std::unique_ptr<Model> Load(const std::string &path);
	public:
		Model() = default;
		void draw();

	public:
		std::vector<ew::Mesh> m_meshes;
	};
}