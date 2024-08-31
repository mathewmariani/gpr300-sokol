#version 300 es

// attributes
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform mat4 light_view_proj;

out vec3 WorldPos;
out vec3 WorldNormal;
out vec4 light_proj_pos;

void main()
{
  WorldPos = vec3(model * vec4(vPosition, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * vNormal;

  light_proj_pos = light_view_proj * vec4(WorldPos, 1.0);
  gl_Position = view_proj * model * vec4(vPosition, 1.0);
}