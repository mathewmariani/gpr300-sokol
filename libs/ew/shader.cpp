/*
*	Author: Eric Winebrenner
*/

#include "shader.h"

// batteries
#include "batteries/opengl.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

namespace ew
{
	/// <summary>
	/// Loads shader source code from a file.
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::string loadShaderSourceFromFile(const std::string& filePath) {
		std::ifstream fstream(filePath);
		if (!fstream.is_open()) {
			printf("Failed to load file %s", filePath.c_str());
			return {};
		}
		std::stringstream buffer;
		buffer << fstream.rdbuf();
		return buffer.str();
	}

	/// <summary>
	/// Creates and compiles a shader object of a given type
	/// </summary>
	/// <param name="shaderType">Expects GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.</param>
	/// <param name="sourceCode">GLSL source code for the shader stage</param>
	/// <returns></returns>
	static unsigned int createShader(GLenum shaderType, const char* sourceCode) {
		//Create a new vertex shader object
		unsigned int shader = glCreateShader(shaderType);
		//Supply the shader object with source code
		glShaderSource(shader, 1, &sourceCode, NULL);
		//Compile the shader object
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			//512 is an arbitrary length, but should be plenty of characters for our error message.
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("Failed to compile shader: %s", infoLog);
		}
		return shader;
	}

	/// <summary>
	/// Creates a shader program with a vertex and fragment shader
	/// </summary>
	/// <param name="vertexShaderSource">GLSL source code for the vertex shader</param>
	/// <param name="fragmentShaderSource">GLSL source code for the fragment shader</param>
	/// <returns></returns>
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
		unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
		unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		unsigned int shaderProgram = glCreateProgram();
		//Attach each stage
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		//Link all the stages together
		glLinkProgram(shaderProgram);
		int success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			printf("Failed to link shader program: %s", infoLog);
		}
		//The linked program now contains our compiled code, so we can delete these intermediate objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return shaderProgram;
	}

	Shader::Shader(const std::string& vertex, const std::string& fragment)
	{
		std::string vertexShaderSource = ew::loadShaderSourceFromFile(vertex.c_str());
		std::string fragmentShaderSource = ew::loadShaderSourceFromFile(fragment.c_str());
		m_id = ew::createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	}

	void Shader::use()const
	{
		if (m_id != 0) {
			glUseProgram(m_id);
		}
	}
	void Shader::setInt(const std::string& name, int v) const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), v);
	}
	void Shader::setFloat(const std::string& name, float v) const
	{
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), v);
	}
	void Shader::setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
	}
	void Shader::setVec2(const std::string& name, const glm::vec2& v) const
	{
		setVec2(name, v.x, v.y);
	}
	void Shader::setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
	}
	void Shader::setVec3(const std::string& name, const glm::vec3& v) const
	{
		setVec3(name, v.x, v.y, v.z);
	}
	void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
	}
	void Shader::setVec4(const std::string& name, const glm::vec4& v) const
	{
		setVec4(name, v.x, v.y, v.z, v.w);
	}
	void Shader::setMat4(const std::string& name, const glm::mat4& m) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
	}
}