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

	typedef void (*obj_request_callback_t)(ew::Shader *shader, const std::string& vertex_src, const std::string& fragment_src);

	struct shader_request_t
	{
		struct {
			const char *vertex;
			const char *fragment;
		} path;
		ew::Shader *shader;
	};

	static void shader_fetch_callback(const sfetch_response_t *response)
	{
		if (response->fetched)
		{
		}
		else if (response->failed)
		{
			printf("[!!!] Failed to load shader file.\n");
		}
	}

	static void load_shader(const shader_request_t &request)
	{
		// auto shader_fetch_callback = [](const sfetch_response_t *response)
		// {
		// 	// this will create the actual shader 
		// };

		// shader_request_t wrapper = {
		// 	.shader = request.shader,
		// 	.callback = request.callback,
		// };

		// sfetch_request_t fetch = {
		// 	.callback = shader_fetch_callback,
		// 	.buffer = SFETCH_RANGE(file_buffer),
		// 	.user_data = SFETCH_RANGE(wrapper),
		// };

		// fetch.path = request.path.vertex;
		// fetch.buffer 
		// sfetch_send(fetch);

		// fetch.path = request.path.fragment;
		// sfetch_send(fetch);
	}

	static void fetch_callback(ew::Shader *shader, const std::string& vertex_src, const std::string& fragment_src)
	{
		ew::createShaderProgram(vertex_src.c_str(), fragment_src.c_str());
	}
}

namespace ew
{
	std::unique_ptr<Shader> Shader::Load(const std::string &vert, const std::string &frag)
	{
		auto ptr = std::make_unique<Shader>();
		load_shader((shader_request_t) {
			.path = {
				.vertex = vert.c_str(),
				.fragment = frag.c_str(),
			},
			// .callback = fetch_callback,
			.shader = ptr.get(),
		});

		return ptr;
	}

	Shader::Shader(const std::string& vertex, const std::string& fragment)
	{
		m_id = ew::createShaderProgram(vertex.c_str(), fragment.c_str());
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
}

