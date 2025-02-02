/*
*	Author: Eric Winebrenner
*/

#include "model.h"

// fastobj
#include "fast_obj/fast_obj.h"

// glm
#include "glm/vec3.hpp"

namespace ew {
	Model::Model(const std::string& filePath)
	{

	}

	void Model::draw()
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].draw();
		}
	}
}
