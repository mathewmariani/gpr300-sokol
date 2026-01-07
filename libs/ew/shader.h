/*
*	Author: Eric Winebrenner
*/

#pragma once

#include <string>
#include <glm/glm.hpp>

namespace ew {
	unsigned int createShaderProgram(unsigned int vertex, unsigned int fragment);

	class Shader {
	public:
		Shader() = default;
		Shader(const std::string& vertex, const std::string& fragment);
		void use()const;
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec2(const std::string& name, const glm::vec2& v) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec3(const std::string& name, const glm::vec3& v) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setVec4(const std::string& name, const glm::vec4& v) const;
		void setMat4(const std::string& name, const glm::mat4& m) const;
	private:
		unsigned int m_id = 0; //Shader program handle
		unsigned int vertex = 0; //Shader program handle
		unsigned int fragment = 0; //Shader program handle
	};
}