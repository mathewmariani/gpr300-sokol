#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// send to fragment
out vec3 world_position;
out vec3 world_normal;

void main()
{
  world_position = vec3(model * vec4(in_position, 1.0));
  world_normal = mat3(transpose(inverse(model))) * in_normal;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}