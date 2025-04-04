/*
*	Author: Eric Winebrenner
*/

#include "shader.h"

// sokol
#include "sokol/sokol_fetch.h"

// opengl
#include <GLES3/gl3.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{

static uint8_t file_buffer[1024 * 1024 * 10];

struct fetch_shader_wrapper
{
	void *ptr;
	GLuint vertex;
	GLuint fragment;
};

struct shader_request_instance_t
{
	int index;
	void *ptr;
};

}

namespace ew
{
	Shader::Shader(const std::string& vertex, const std::string& fragment)
	{
		fetch_shader_wrapper w = {
			.ptr = this,
			.vertex = 0,
			.fragment = 0,
		};

		const char* stages[2] = {
			vertex.c_str(),
			fragment.c_str(),
		};

		for (auto i = 0; i < 2; ++i) {
			shader_request_instance_t instance = {
				.index = i,
				.ptr = this,
			};

			sfetch_send((sfetch_request_t){
				.path = stages[i],
				.callback = fetchCallback,
				.buffer = SFETCH_RANGE(file_buffer),
				.user_data = SFETCH_RANGE(instance),
			});
		}
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
	unsigned int createShaderProgram(unsigned int vertex, unsigned int fragment) {
		unsigned int shaderProgram = glCreateProgram();

		//Attach each stage
		glAttachShader(shaderProgram, vertex);
		glAttachShader(shaderProgram, fragment);
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
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return shaderProgram;
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

	void Shader::fetchCallback(const sfetch_response_t* response)
	{
		if (!response->fetched)
		{
			printf("[!!!] Failed to load shader file (%s)\n", response->path);
			return;
		}

		char* source = (char*)malloc(response->data.size + 1);
		if (source) {
			memcpy(source, response->data.ptr, response->data.size);
			source[response->data.size] = '\0';
		}

		// create shader stage
		auto instance = static_cast<shader_request_instance_t*>(response->user_data);
		auto* self = static_cast<Shader*>(instance->ptr);
		GLenum stage = (instance->index == 0) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

		switch (stage)
		{
			case GL_VERTEX_SHADER:
				self->vertex = createShader(stage, source);
				break;
			case GL_FRAGMENT_SHADER:
				self->fragment = createShader(stage, source);
				break;
		};

		if (self->vertex != 0 && self->fragment != 0)
		{
			self->m_id = createShaderProgram(self->vertex, self->fragment);
		}

		free(source);
	}
}