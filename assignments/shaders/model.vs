#version 300 es

// attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

out vec3 WorldPos;
out vec3 WorldNormal;
out vec2 TexCoords;

void main()
{
  WorldPos = vec3(model * vec4(position, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * normal;
  TexCoords = texcoord;
  gl_Position = view_proj * model * vec4(position, 1.0);
}