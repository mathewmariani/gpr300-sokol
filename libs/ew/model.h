/*
*	Author: Eric Winebrenner
*/

#pragma once

#include "mesh.h"
#include <string>
#include <vector>

namespace ew {
	class Model {
	public:
		Model(const std::string& filePath);
		void draw();
	private:
		std::vector<ew::Mesh> m_meshes;
	};
}