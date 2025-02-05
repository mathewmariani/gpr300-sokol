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
	fetch_shader_wrapper *wrapper;
};

}

namespace ew
{
	Shader::Shader(const std::string& vertex, const std::string& fragment)
	{
		m_id = ew::createShaderProgram(vertex.c_str(), fragment.c_str());

		// fetch_shader_wrapper w = {
		// 	.ptr = this,
		// 	.vertex = 0,
		// 	.fragment = 0,
		// };

		// const char* stages[2] = {
		// 	vertex.c_str(),
		// 	fragment.c_str(),
		// };

		// for (auto i = 0; i < 2; ++i) {
		// 	shader_request_instance_t instance = {
		// 		.index = i,
		// 		.wrapper = &w,
		// 	};

		// 	sfetch_send((sfetch_request_t){
		// 		.path = stages[i],
		// 		.callback = fetchCallback,
		// 		.buffer = SFETCH_RANGE(file_buffer),
		// 		.user_data = SFETCH_RANGE(instance),
		// 	});
		// }
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

	void Shader::use()const
	{
		glUseProgram(m_id);
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
		GLenum stage = (instance->index == 0) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

		printf("IM HERE");
		switch (stage)
		{
			case GL_VERTEX_SHADER:
				instance->wrapper->vertex = createShader(stage, source);
				break;
			case GL_FRAGMENT_SHADER:
				instance->wrapper->fragment = createShader(stage, source);
				break;
		};

		printf("shader values (%d, %d)", instance->wrapper->vertex, instance->wrapper->fragment);

		if (instance->wrapper->vertex != 0 && instance->wrapper->fragment != 0)
		{
			auto* self = (Shader*)(static_cast<fetch_shader_wrapper*>(response->user_data))->ptr;
			self->m_id = glCreateProgram();

			glAttachShader(self->m_id, instance->wrapper->vertex);
			glAttachShader(self->m_id, instance->wrapper->fragment);
			glLinkProgram(self->m_id);

			int success;
			glGetProgramiv(self->m_id, GL_LINK_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetProgramInfoLog(self->m_id, 512, NULL, infoLog);
				printf("Failed to link shader program: %s", infoLog);
			}

			glDeleteShader(instance->wrapper->vertex);
			glDeleteShader(instance->wrapper->fragment);
		}

		free(source);
	}
}